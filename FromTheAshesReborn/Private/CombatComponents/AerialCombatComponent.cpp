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
		UE_LOG(LogTemp, Warning, TEXT("Component active for: %f seconds"), GetTotalComponentActiveTime());

	}
}

void UAerialCombatComponent::ClearStateAndVariables()
{
	UE_LOG(LogTemp, Warning, TEXT("Resetting Vars"));
	IsComponentActive = false;
	CMC->GravityScale = 4.0f;
	FTAAbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("AerialCombatTag.AttackCounter")));
	AttackCounterGravityMultiplier = 0.0f;
	AttackCounter = 0;
	AttackLastResetTime = GetWorld()->GetTimeSeconds();
	UE_LOG(LogTemp, Warning, TEXT("Final: %f seconds"), GetTotalComponentActiveTime());
	TotalAirTime = 0.0f;

}

void UAerialCombatComponent::InitializeStateAndVariables()
{
	IsComponentActive = true;
	TotalAirTime = GetWorld()->GetTimeSeconds();
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

	if (AttackCounter <= 3)
	{
		CMC->GravityScale = 0;
	}
	else if (AttackCounter > 3 && AttackCounter <= 6)
	{
		CMC->GravityScale = 0.2f;
	}
	else if (AttackCounter > 6 && AttackCounter <= 9)
	{
		CMC->GravityScale = 1.0f;
	}
	else
	{
		CMC->GravityScale = 4.0f;
	}

	UE_LOG(LogTemp, Warning, TEXT("Time since last aerial attack: %f seconds"), GetAttackElapsedTime());

	ResetAttackTimer();

	return 0.0f;
}


void UAerialCombatComponent::CalculateTimeSpentGravityMultiplier()
{
}

void UAerialCombatComponent::ResetAttackTimer()
{
	AttackLastResetTime = GetWorld()->GetTimeSeconds();
}

float UAerialCombatComponent::GetAttackElapsedTime() const
{
	return GetWorld()->GetTimeSeconds() - AttackLastResetTime;
}

float UAerialCombatComponent::GetTotalComponentActiveTime() const
{
	if (TotalAirTime == 0.0f)
	{
		return 0.0f;
	}

	return GetWorld()->GetTimeSeconds() - TotalAirTime;
}