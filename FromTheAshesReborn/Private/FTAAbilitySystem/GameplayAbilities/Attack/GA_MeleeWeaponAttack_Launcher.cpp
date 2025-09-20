#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack_Launcher.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Camera/CameraSystemComponent.h"
#include "MotionWarpingComponent.h"
#include "CombatComponents/AerialCombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/EngineCustomTimeStep.h"
#include "EventObjects/LaunchEventObject.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/AT_LaunchCharacterAndWait.h"
#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_ReceiveHit.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
#include "Player/FTAPlayerController.h"
#include "Player/FTAPlayerState.h"

UGA_MeleeWeaponAttack_Launcher::UGA_MeleeWeaponAttack_Launcher()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_MeleeWeaponAttack_Launcher::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);

}
bool UGA_MeleeWeaponAttack_Launcher::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	//Return true if owner is not player
	AFTAPlayerState* PS = Cast<AFTAPlayerState>(ActorInfo->OwnerActor.Get());
	if(ActorInfo->OwnerActor.Get() != PS)
	{
		return true;
	}
	
	if(!PS->HardLockedTargetActor)
	{
		return false;
	}
	
	float AngleOfInput = ULockOnFunctionLibrary::AngleFromInputVectorToLockedTarget(ActorInfo->AvatarActor.Get(), PS->HardLockedTargetActor);
	
	ELockOnInputOrientationDirection InputOrientation = ULockOnFunctionLibrary::GetOrientationOfInput(AngleOfInput);
	if(InputOrientation != ELockOnInputOrientationDirection::Backward)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_MeleeWeaponAttack_Launcher::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	LaunchTask = UAT_LaunchCharacterAndWait::AT_LaunchCharacterAndWait(
		this,
		LauncherVerticalDistance,
		LauncherDuration,
		StallDuration, LaunchOffset);

	UCameraSystemComponent* CSC = GetFTACharacterFromActorInfo()->FindComponentByClass<UCameraSystemComponent>();
	if(!CSC)
	{
		return;
	}
}

void UGA_MeleeWeaponAttack_Launcher::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_MeleeWeaponAttack_Launcher::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	GetFTACharacterFromActorInfo()->MotionWarpingComponent->RemoveWarpTarget(WarpTargetName);

	if(EnableAerialCombatEffect)
	{
		UAerialCombatComponent* ACC = GetFTACharacterFromActorInfo()->FindComponentByClass<UAerialCombatComponent>();

		if(!ACC->IsValidLowLevel())
		{
			UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack_Launcher::EndAbility - ACC invalid"))
		}
		ACC->ActivateFromLauncher = true;
		FGameplayEffectSpecHandle GEHandle = MakeOutgoingGameplayEffectSpec(EnableAerialCombatEffect, 1.0f);
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
	}
	
}

void UGA_MeleeWeaponAttack_Launcher::SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> CurrentHitReactionStruct)
{
	
	ULaunchEventObject* LaunchInfoObj = NewObject<ULaunchEventObject>(this);
	LaunchInfoObj->LaunchData.VerticalDistance = LauncherVerticalDistance;
	LaunchInfoObj->LaunchData.LaunchDuration = LauncherDuration;
	LaunchInfoObj->LaunchData.StallDuration = StallDuration;
	LaunchInfoObj->LaunchData.Offset = LaunchOffset;
	LaunchInfoObj->HitData.Instigator = GetFTACharacterFromActorInfo();
	
	FGameplayEventData OnLaunchHitEventData;
	OnLaunchHitEventData.OptionalObject = LaunchInfoObj;
	
	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
	
	OnLaunchHitEventData.Instigator = GetAvatarActorFromActorInfo();
	OnLaunchHitEventData.Target = TargetActor;
	OnLaunchHitEventData.ContextHandle.AddHitResult(*TargetDataHandle.Get(0)->GetHitResult());
	
	if (CurrentHitReactionStruct)
	{
		const UGA_ReceiveHit* const CDO = CurrentHitReactionStruct->GetDefaultObject<UGA_ReceiveHit>();
		if (CDO)
		{
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CDO->HitTag))
			{
				OnLaunchHitEventData.EventTag = CDO->HitTag;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::SendMeleeHitGameplayEvents - HitReactionTag is invalid"));
			}
		}
		else
		{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::SendMeleeHitGameplayEvents - CDO is invalid"));
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::SendMeleeHitGameplayEvents - CurrentHitReactionStruct is invalid"));
		
	}

	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, OnLaunchHitEventData.EventTag, OnLaunchHitEventData);
}


void UGA_MeleeWeaponAttack_Launcher::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_MeleeWeaponAttack_Launcher::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_MeleeWeaponAttack_Launcher::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);

	if(!UTagValidationFunctionLibrary::IsRegisteredGameplayTag(EventTag))
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack_Launcher::EventMontageReceived - EventTag is invalid"))
		return;
	}
	
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("CombatMovementTag.Launch.Vertical")))
	{
		if (LaunchTask)
		{
			LaunchTask->OnLaunchComplete.AddDynamic(this, &UGA_MeleeWeaponAttack_Launcher::OnLaunchComplete);
			LaunchTask->ReadyForActivation();
		}
	}
	
	
}

void UGA_MeleeWeaponAttack_Launcher::OnLaunchComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
