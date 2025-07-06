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
		ElapsedTime += DeltaTime;
		UE_LOG(LogTemp, Warning, TEXT("ElapsedTime: %f"), ElapsedTime);
	}
}

void UAerialCombatComponent::ClearStateAndVariables()
{
	UE_LOG(LogTemp, Warning, TEXT("Resetting Vars"))
	IsComponentActive = false;
	CMC->GravityScale = 4.0f;
	FTAAbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("AerialCombatTag.AttackCounter")));
	AttackCounterGravityMultiplier = 0.0f;
	AttackCounter = 0;
	GetWorld()->GetTimerManager().ClearTimer(AerialCombatTimerHandle);
	ElapsedTime = 0.0f;
}

void UAerialCombatComponent::InitializeStateAndVariables()
{
	IsComponentActive = true;
	CMC->GravityScale = 0.0f;
	// GetWorld()->GetTimerManager().SetTimer(AerialCombatTimerHandle, this, &UAerialCombatComponent::CalculateTimeSpentGravityMultiplier, 0.1f, true);
	
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
	CMC->GravityScale = AttackCounter;
	return 0.0;
}

float UAerialCombatComponent::CalculateTimeSpentGravityMultiplier()
{
	return 0.0f;
}
