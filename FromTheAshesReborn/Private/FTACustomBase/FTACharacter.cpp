#include "FTACustomBase/FTACharacter.h"
#include "FTAAbilitySystem/FTAGameplayAbility.h"
#include "GameplayAbilitySpec.h"
#include "FTAAbilitySystem/FTAAbilitySystemComponent.h"

AFTACharacter::AFTACharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AFTACharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFTACharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFTACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* AFTACharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFTAAttributeSet* AFTACharacter::GetAttributeSet() const
{
	return AttributeSet;
}

int32 AFTACharacter::GetAbilityLevel(EGAbilityInputID AbilityID)
{
	return 1;
}

void AFTACharacter::InitDefaultAttributes() const
{
	if(!AbilitySystemComponent || !DefaultAttributeEffect) { return; }

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1.0f, EffectContext);

	if(SpecHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AFTACharacter::GiveDefaultAbilities()
{
	if(!AbilitySystemComponent) return;

	if(!HasAuthority()) return;
	for(TSubclassOf<UFTAGameplayAbility>& StartupAbility : DefaultAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
		 	//AbilitySystemComponent->bCharacterAbilitiesGiven = true;
	}
}
