#include "FTAAbilitySystem/GameplayAbilities/GA_Jump.h"
#include "FTACustomBase/FTACharacter.h"
#include "FTACustomBase/FTAEnums.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Jump::UGA_Jump()
{
    AbilityInputID = EAbilityInputID::Jump;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Jump::AbilityTickComponent()
{
    Super::AbilityTickComponent();

    FVector JumpLurchInfluencer = FVector(0.0f, 0.0f, 0.0f);
    // UE_LOG(LogTemp, Warning, TEXT("LastInputVectorString: %s"), *CMC->GetLastInputVector().GetSafeNormal().ToString());
    
    // FVector WallForceVector = CMC->GetLastInputVector().GetSafeNormal() * (20 * 3.6f * 0.2778f * CMC->Mass * 100.0f);
    // CMC->AddForce(WallForceVector);
    
    CMC->AddForce(CMC->GetLastInputVector().GetSafeNormal(0.0001) * 600000.0f);
    

}

void UGA_Jump::OnCharacterLanded(const FHitResult& Hit)
{
    if (GetFTACharacterFromActorInfo())
    {
        GetFTACharacterFromActorInfo()->LandedDelegate.RemoveDynamic(this, &UGA_Jump::OnCharacterLanded);
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
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
        
        if (!GetFTACharacterFromActorInfo())
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        GetFTACharacterFromActorInfo()->LandedDelegate.AddDynamic(this, &UGA_Jump::OnCharacterLanded);

        CMC = Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent);
        if(!CMC)
        {
            UE_LOG(LogTemp, Error, TEXT("GA_Jump::ActivateAbility - CMC is NULL"));
        }
        
        GetFTACharacterFromActorInfo()->Jump();
        StartJumpLurchWindowTimer();
        
    }
}

bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }
    
    ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
    if(Character->JumpCurrentCount > 2)
    {
        return false;
    }
    
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

void UGA_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Jump::StartJumpLurchWindowTimer()
{
    JumpLurchElapsedTime = 0.0f;
    GetWorld()->GetTimerManager().SetTimer(FJumpLurchWindowTimerHandle, this, &UGA_Jump::UpdateJumpLurchStrength, 0.016, true);
}

void UGA_Jump::UpdateJumpLurchStrength()
{
    JumpLurchElapsedTime += 0.01f;
    if(JumpLurchElapsedTime >= JumpLurchWindowDuration)
    {
        JumpLurchElapsedTime = JumpLurchWindowDuration;
        GetWorld()->GetTimerManager().ClearTimer(FJumpLurchWindowTimerHandle);
    }

    float Alpha = JumpLurchElapsedTime / JumpLurchWindowDuration;
    float CurrentValue = FMath::Lerp(JumpLurchMaxMultiplier, JumpLurchMinMultiplier, Alpha);
    UE_LOG(LogTemp, Warning, TEXT("CurrentValue: %f"), CurrentValue);
}
