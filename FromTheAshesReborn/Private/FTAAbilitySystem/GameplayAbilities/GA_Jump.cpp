#include "FTAAbilitySystem/GameplayAbilities/GA_Jump.h"
#include "FTACustomBase/FTACharacter.h"
#include "FTACustomBase/FTAEnums.h"
#include "GameFramework/Character.h"

UGA_Jump::UGA_Jump()
{
    AbilityInputID = EAbilityInputID::Jump;
    // InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
        Character->Jump();
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }
}

bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    // Check if the base ability can activate
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }

    // Ensure the character can jump
    const AFTACharacter* Character = CastChecked<AFTACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
    return Character && Character->CanJump();
}

void UGA_Jump::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

    if (ActorInfo && ActorInfo->AvatarActor.IsValid())
    {
        ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
        Character->StopJumping();
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
