#include "Player/FTAPlayerState.h"

#include "DataAsset/FTACharacterData.h"
#include "FTAAbilitySystem/AbilitySets/FTAAbilitySet.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/Globals/FTAAbilitySystemGlobals.h"
#include "Player/PlayerCharacter.h"

AFTAPlayerState::AFTAPlayerState(const FObjectInitializer& ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UFTAAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UFTAAttributeSet>(TEXT("AttributeSet"));

	NetUpdateFrequency = 100.0f;
}

UAbilitySystemComponent* AFTAPlayerState::GetAbilitySystemComponent() const
{
	return GetFTAAbilitySystemComponent();
}

void AFTAPlayerState::BeginPlay()
{
	Super::BeginPlay();

	AddAbilitiesToPlayerASC();

	if (AbilitySystemComponent)
	{
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentHealthAttribute()).AddUObject(this, &AFTAPlayerState::HealthChanged);
	}
}

void AFTAPlayerState::AddAbilitiesToPlayerASC()
{
	APlayerCharacter* PC = Cast<APlayerCharacter>(GetPawn());

	for (const UFTAAbilitySet* AbilitySet : PC->FTACharacterData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
}

UFTAAttributeSet* AFTAPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

bool AFTAPlayerState::IsAlive() const
{
	return GetCurrentHealth() > 0.0f;
}

float AFTAPlayerState::GetCurrentHealth() const
{
	return AttributeSet->GetCurrentHealth();
}

float AFTAPlayerState::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

float AFTAPlayerState::GetMoveSpeed() const
{
	return AttributeSet->GetMoveSpeed();
}

void AFTAPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	APlayerCharacter* PC = Cast<APlayerCharacter>(GetPawn());
	if (IsValid(PC) && !IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		if (PC)
		{
			PC->FinishDying();
		}
	}
}
