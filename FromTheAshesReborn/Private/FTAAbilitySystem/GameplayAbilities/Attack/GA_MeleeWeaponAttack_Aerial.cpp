#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack_Aerial.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

bool UGA_MeleeWeaponAttack_Aerial::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		UE_LOG(LogTemp, Warning, TEXT("Parent failed"));
		return false;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		return false;
	}

	return Character->GetCharacterMovement()->IsFalling() || Character->GetCharacterMovement()->IsFlying();
}
