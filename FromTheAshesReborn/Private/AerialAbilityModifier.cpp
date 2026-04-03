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

void UAerialAbilityModifier::EnableAerialComponent(UGameplayAbility* InGameplayAbility,
	UAerialCombatComponent* AirCombatComponent, UAbilitySystemComponent* ASC)
{
	if(AirCombatComponent && AirCombatComponent->IsValidLowLevel())
	{
		AirCombatComponent->EnableComponent(MOVE_Falling);
	}
}
