#include "AerialAbilityModifier.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbility.h"
#include "CombatComponents/AerialCombatComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

bool UAerialAbilityModifier::CanActivate(TObjectPtr<ACharacter> Character) const
{
	return IsCharacterAirborne(Character);
}

bool UAerialAbilityModifier::IsCharacterAirborne(TObjectPtr<ACharacter> Character) const
{
	return Character->GetCharacterMovement()->IsFalling() || Character->GetCharacterMovement()->IsFlying();
}

void UAerialAbilityModifier::EnableAerialComponent(TObjectPtr<UGameplayAbility> InGameplayAbility,
	TObjectPtr<UAerialCombatComponent> AirCombatComponent, TObjectPtr<UAbilitySystemComponent> ASC)
{
	if(AirCombatComponent->EnableAerialCombatEffect)
	{
		FGameplayEffectSpecHandle GEHandle = InGameplayAbility->MakeOutgoingGameplayEffectSpec(AirCombatComponent->EnableAerialCombatEffect, 1.0f);
		ASC->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
	}
}
