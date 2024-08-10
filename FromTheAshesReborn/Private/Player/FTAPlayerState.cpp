#include "Player/FTAPlayerState.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/Globals/FTAAbilitySystemGlobals.h"
#include "Player/PlayerCharacter.h"

AFTAPlayerState::AFTAPlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UFTAAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSet = CreateDefaultSubobject<UFTAAttributeSet>(TEXT("AttributeSet"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 100.0f;

	DeadTag = FGameplayTag::RequestGameplayTag("Condition.Health.Dead");
}

UAbilitySystemComponent* AFTAPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
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

void AFTAPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentHealthAttribute()).AddUObject(this, &AFTAPlayerState::HealthChanged);
	}
}

void AFTAPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	// Check for and handle knockdown and death
	APlayerCharacter* PC = Cast<APlayerCharacter>(GetPawn());
	if (IsValid(PC) && !IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		if (PC)
		{
			PC->FinishDying();
		}
	}
}
