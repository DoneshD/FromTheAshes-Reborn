#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack_Grounded.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

bool UGA_MeleeWeaponAttack_Grounded::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);

	bool IsGrounded = !Character->GetCharacterMovement()->IsFalling() && !Character->GetCharacterMovement()->IsFlying();

	if(!IsGrounded)
	{
		return false;
	}

	return true;
}
