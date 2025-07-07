#include "CombatComponents/AerialCombatComponent.h"

#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


UAerialCombatComponent::UAerialCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UAerialCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	FTACharacter = Cast<AFTACharacter>(GetOwner());

	if(!FTACharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UAerialCombatComponent::BeginPlay() - FTACharacter is null"));
		return;
	}

	FTAAbilitySystemComponent = FTACharacter->GetFTAAbilitySystemComponent();

	if(!FTAAbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UAerialCombatComponent::BeginPlay() - FTAAbilitySystemComponent is null"));
		return;
	}

	CMC = Cast<UCharacterMovementComponent>(FTACharacter->GetMovementComponent());

	if(!CMC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAerialCombatComponent::BeginPlay() - CMC is Null"));
		return;
	}

	FDelegateHandle Handle = FTAAbilitySystemComponent->RegisterGameplayTagEvent(EnableTag, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &UAerialCombatComponent::EnableComponent);

	FDelegateHandle HandleTwo = FTAAbilitySystemComponent->RegisterGameplayTagEvent(AerialAttackCounterTag, EGameplayTagEventType::AnyCountChange)
		.AddUObject(this, &UAerialCombatComponent::AddAttackCounterTag);
	
}

void UAerialCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(IsComponentActive)
	{
		if (CMC->Velocity.Z > 0.f)
		{
			CMC->Velocity.Z = 0.f;
		}
		TotalAirTime += DeltaTime;
		CMC->GravityScale = CalculateTimeSpentGravityMultiplier();
		PrintGravity();
	}
}

void UAerialCombatComponent::ClearStateAndVariables()
{
	IsComponentActive = false;
	CMC->GravityScale = 4.0f;
	AttackCounterGravityMultiplier = 0.0f;
	AttackCounter = 0;
	AttackLastResetTime = GetWorld()->GetTimeSeconds();
	TotalAirTime = 0.0f;
	FTAAbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("AerialCombatTag.AttackCounter")));

}

void UAerialCombatComponent::InitializeStateAndVariables()
{
	IsComponentActive = true;
	ResetAttackTimer();
	
}

void UAerialCombatComponent::EnableComponent(const FGameplayTag InEnableTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		InitializeStateAndVariables();
	}
	else
	{
		ClearStateAndVariables();
	}
}

void UAerialCombatComponent::SetGravity(float NewGravity)
{
	CMC->GravityScale = NewGravity;
}

void UAerialCombatComponent::PrintGravity()
{
	UE_LOG(LogTemp, Warning, TEXT("Gravity: %f"), CMC->GravityScale)
}

void UAerialCombatComponent::AddAttackCounterTag(const FGameplayTag InAttackCounterTag, int32 NewCount)
{
	if (NewCount > PreviousCount)
	{
		CalculateAttackCountGravityMultiplier(NewCount);
	}

	PreviousCount = NewCount;
}

float UAerialCombatComponent::CalculateAttackCountGravityMultiplier(int InNewCount)
{
	AttackCounter = InNewCount;
	
	// if (AttackCounter <= 3)
	// {
	// 	CMC->GravityScale = CMC->GravityScale * 1.1;
	// }
	// else if (AttackCounter > 3 && AttackCounter <= 6)
	// {
	// 	CMC->GravityScale = CMC->GravityScale * 1.5;
	// }
	// else if (AttackCounter > 6 && AttackCounter <= 9)
	// {
	// 	CMC->GravityScale = CMC->GravityScale * 2;
	// }
	// else
	// {
	// 	CMC->GravityScale = 4.0f;
	// }

	ResetAttackTimer();

	return 0.0f;
}


float UAerialCombatComponent::CalculateTimeSpentGravityMultiplier() const
{
	return FMath::Square(TotalAirTime) * TimeGravityMultiplier;
}

void UAerialCombatComponent::ResetAttackTimer()
{
	AttackLastResetTime = GetWorld()->GetTimeSeconds();
}

float UAerialCombatComponent::GetAttackElapsedTime() const
{
	return GetWorld()->GetTimeSeconds() - AttackLastResetTime;
}