﻿#include "FTAAbilitySystem//AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "GameFramework/Character.h"

UFTAAT_PlayMontageAndWaitForEvent::UFTAAT_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
}

UFTAAbilitySystemComponent* UFTAAT_PlayMontageAndWaitForEvent::GetTargetASC() const
{
	return Cast<UFTAAbilitySystemComponent>(AbilitySystemComponent);
}

void UFTAAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted) const
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (Montage == MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if(Character)
			{
				Character->SetAnimRootMotionTranslationScale(1.f);
			}
		}
	}

	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UFTAAT_PlayMontageAndWaitForEvent::OnAbilityCancelled()
{
	if (StopPlayingMontage())
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UFTAAT_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	EndTask();
}

void UFTAAT_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}

UFTAAT_PlayMontageAndWaitForEvent* UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(UGameplayAbility* OwningAbility,
FName TaskInstanceName, UAnimMontage* MontageToPlay, FGameplayTagContainer EventTags, float Rate, FName StartSection, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UFTAAT_PlayMontageAndWaitForEvent* MontageTask = NewAbilityTask<UFTAAT_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);

	if (!MontageTask)
	{
		UE_LOG(LogTemp, Error, TEXT("UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent - MontageTask is null"));
		return nullptr;
	}
	MontageTask->MontageToPlay = MontageToPlay;
	MontageTask->EventTags = EventTags;
	MontageTask->Rate = Rate;
	MontageTask->StartSection = StartSection;
	MontageTask->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MontageTask->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

	return MontageTask;
}


void UFTAAT_PlayMontageAndWaitForEvent::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	bool bPlayedMontage = false;


	if (UFTAAbilitySystemComponent* FTAAbilitySystemComponent = GetTargetASC())
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			EventHandle = FTAAbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UFTAAT_PlayMontageAndWaitForEvent::OnGameplayEvent));
			
			if (FTAAbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection) > 0.f)
			{
				if (ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}
			
				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UFTAAT_PlayMontageAndWaitForEvent::OnAbilityCancelled);
			
				BlendingOutDelegate.BindUObject(this, &UFTAAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);
			
				MontageEndedDelegate.BindUObject(this, &UFTAAT_PlayMontageAndWaitForEvent::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
				
				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character && (Character->GetLocalRole() == ROLE_Authority ||
					(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}
				bPlayedMontage = true;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UFTAAbilityTask_PlayMontageAndWaitForEvent call to PlayMontage failed!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UFTAAbilityTask_PlayMontageAndWaitForEvent called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFTAAbilityTask_PlayMontageAndWaitForEvent called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void UFTAAT_PlayMontageAndWaitForEvent::ExternalCancel()
{
	OnAbilityCancelled();
	Super::ExternalCancel();
}

void UFTAAT_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}

	if (UFTAAbilitySystemComponent* FTAAbilitySystemComponent = GetTargetASC())
	{
		FTAAbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(AbilityEnded);

}

bool UFTAAT_PlayMontageAndWaitForEvent::StopPlayingMontage() const
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)
	{
		return false;
	}

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	UFTAAbilitySystemComponent* FTAAbilitySystemComponent  = GetTargetASC();
	if (!FTAAbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASC NULL"))
		return false;
	}
	if (FTAAbilitySystemComponent && Ability)
	{
		if (FTAAbilitySystemComponent->GetAnimatingAbility() == Ability
			&& FTAAbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
		{
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance)
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}
	
			FTAAbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}
	return false;
}

FString UFTAAT_PlayMontageAndWaitForEvent::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}


