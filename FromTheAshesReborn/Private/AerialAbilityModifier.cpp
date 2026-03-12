#include "AerialAbilityModifier.h"

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
