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
    GetWorld()->GetTimerManager().SetTimer(FJumpLurchWindowTimerHandle, this, &UGA_Jump::UpdateJumpLurch, 0.016, true);
}

void UGA_Jump::UpdateJumpLurch()
{
    JumpLurchElapsedTime += 0.016f;
    if(JumpLurchElapsedTime >= JumpLurchWindowDuration)
    {
        JumpLurchElapsedTime = JumpLurchWindowDuration;
        GetWorld()->GetTimerManager().ClearTimer(FJumpLurchWindowTimerHandle);
    }

    UpdateJumpLurchStrength();
    UpdateCounterLurchForce();
}

void UGA_Jump::UpdateJumpLurchStrength()
{
    float Alpha = FMath::Pow(JumpLurchElapsedTime / JumpLurchWindowDuration, 0.5f);

    CurrentJumpLurchStrengthMultiplier = FMath::Lerp(JumpLurchMaxMultiplier, JumpLurchMinMultiplier, Alpha);

    UE_LOG(LogTemp, Warning, TEXT("LurchForce: %f"), JumpLurchBaseStrength * CurrentJumpLurchStrengthMultiplier);
    
    FVector LurchForce = CMC->GetLastInputVector().GetSafeNormal(0.0001f) * (JumpLurchBaseStrength * CurrentJumpLurchStrengthMultiplier);
    CMC->AddForce(LurchForce);
}

void UGA_Jump::UpdateCounterLurchForce()
{
    float Alpha = FMath::Pow(JumpLurchElapsedTime / JumpLurchWindowDuration, 0.5f); // Square root curve
    CounterCurrentJumpLurchStrengthMultiplier = FMath::Lerp(JumpLurchMaxMultiplier, JumpLurchMinMultiplier, Alpha);
    
    FVector CounterLurchForce = -CMC->GetLastInputVector().GetSafeNormal(0.0001f) * (JumpLurchBaseStrength * CounterCurrentJumpLurchStrengthMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("CounterLurchForce: %f"), JumpLurchBaseStrength * CurrentJumpLurchStrengthMultiplier);
    CMC->AddForce(CounterLurchForce);
    
}
