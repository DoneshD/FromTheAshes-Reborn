#include "Player/FTAPlayerState.h"
#include "FTAAbilitySystem/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/FTAAttributeSet.h"

AFTAPlayerState::AFTAPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;

	NetUpdateFrequency = 100.0f;

	AbilitySystemComponent = CreateDefaultSubobject<UFTAAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UFTAAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AFTAPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFTAAttributeSet* AFTAPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}
