﻿#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedMeleeAttack.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"
#include "Interfaces/MeleeCombatantInterface.h"
#include "DataAsset/MeleeAttackDataAsset.h"

UGA_GroundedMeleeAttack::UGA_GroundedMeleeAttack()
{
	
}

void UGA_GroundedMeleeAttack::PrintCurrentComboContainer()
{
	IMeleeCombatantInterface* MeleeCombatantInterface = Cast<IMeleeCombatantInterface>(GetAvatarActorFromActorInfo());

	for (const FGameplayTag& Tag : MeleeCombatantInterface->GetCurrentComboContainer())
	{
		if(*Tag.ToString())
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentComboTagContainer Tag: %s"), *Tag.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MatchingContainer Tag is NULL"));
		}
	}
}

void UGA_GroundedMeleeAttack::ResetGroundedMeleeAttack()
{
	IMeleeCombatantInterface* MeleeCombatantInterface = Cast<IMeleeCombatantInterface>(GetAvatarActorFromActorInfo());

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(LightInput,
		EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(HeavyInput,
			EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(LightInput);
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(HeavyInput);

	MeleeCombatantInterface->GetCurrentComboContainer().Reset();
	MeleeCombatantInterface->SetCurrentComboIndex(0);
	
	GetWorld()->GetTimerManager().ClearTimer(FLightComboWindowTimer);
	GetWorld()->GetTimerManager().ClearTimer(FHeavyComboWindowTimer);

	LastInputWasLight = false;
	LastInputWasHeavy = false;

	//UE_LOG(LogTemp, Warning, TEXT("Reset Attack"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_GroundedMeleeAttack::LightComboSavedTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(LightInput))
	{
		UE_LOG(LogTemp, Warning, TEXT("Next Light"));
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(LightInput);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		//Change later to gameplay event instead of input ID
		ProceedToNextCombo(7);
	}
}

void UGA_GroundedMeleeAttack::HeavyComboSavedTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	UE_LOG(LogTemp, Warning, TEXT("HeavyTagChanged"));
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(HeavyInput))
	{
		UE_LOG(LogTemp, Warning, TEXT("Next Heavy"));
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(HeavyInput);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		//Change later to gameplay event instead of input ID
		ProceedToNextCombo(8);
	}
}

bool UGA_GroundedMeleeAttack::FindMatchingTagContainer(const TArray<TObjectPtr<UMeleeAttackDataAsset>>& GroundedAttackDataAssets, TObjectPtr<UMeleeAttackDataAsset>& OutMatchingDataAsset)
{
	IMeleeCombatantInterface* MeleeCombatantInterface = Cast<IMeleeCombatantInterface>(GetAvatarActorFromActorInfo());
	
	for (int32 Index = 0; Index < GroundedAttackDataAssets.Num(); ++Index)
	{
		if (GroundedAttackDataAssets[Index])
		{
			if (MeleeCombatantInterface->GetCurrentComboContainer().HasAll(GroundedAttackDataAssets[Index]->RequiredTags))
			{
				if(GroundedAttackDataAssets[Index]->RequiredIndex == MeleeCombatantInterface->GetCurrentComboIndex())
				{
					OutMatchingDataAsset = GroundedAttackDataAssets[Index];
					return true;
				}
			}
		}
	}
	
	return false;
}

void UGA_GroundedMeleeAttack::ProceedToNextCombo(int32 IDToActivate)
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponentFromActorInfo()->GetActivatableAbilities())
	{
		if (Spec.Ability)
		{
			if(Spec.InputID == IDToActivate)
			{
				GetAbilitySystemComponentFromActorInfo()->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void UGA_GroundedMeleeAttack::PerformGroundedMeleeAttack(TArray<TObjectPtr<UMeleeAttackDataAsset>> GroundedAttackDataAssets)
{
	IMeleeCombatantInterface* MeleeCombatantInterface = Cast<IMeleeCombatantInterface>(GetAvatarActorFromActorInfo());

	TObjectPtr<UMeleeAttackDataAsset> MatchingDataAsset;
	bool DataAssetFound = FindMatchingTagContainer(GroundedAttackDataAssets, MatchingDataAsset);
	
	if(!DataAssetFound)
	{
		UE_LOG(LogTemp, Error, TEXT("DA NOT found"));
		ResetGroundedMeleeAttack();
		return;
	}
	
	if(!MatchingDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("MatchingDataAsset is NULL"));
		ResetGroundedMeleeAttack();
		return;
	}

	AttackMontageToPlay = MatchingDataAsset->MontageToPlay;
	
	if(AttackMontageToPlay)
	{
		FGameplayTag AttackIndentiferTag = MatchingDataAsset->AttackIndentiferTag;
		MeleeCombatantInterface->GetCurrentComboContainer().AddTag(AttackIndentiferTag);
		MeleeCombatantInterface->SetCurrentComboIndex(MeleeCombatantInterface->GetCurrentComboIndex() + 1);

		PlayAttackMontage(AttackMontageToPlay);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AttackMontageToPlay is NULL"));
		ResetGroundedMeleeAttack();
	}
}

void UGA_GroundedMeleeAttack::PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage)
{
	AttackMontageToPlay = AttackMontage;
	if(AttackMontageToPlay)
	{
		Task = UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, AttackMontageToPlay, FGameplayTagContainer(),
		1.0f, NAME_None, false, 1.0f);
		Task->OnBlendOut.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCompleted);
		Task->OnCompleted.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCompleted);
		Task->OnInterrupted.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCancelled);
		Task->OnCancelled.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCancelled);
		Task->EventReceived.AddDynamic(this, &UGA_GroundedMeleeAttack::EventReceived);
		
		Task->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No attack montage"));
	}
}

void UGA_GroundedMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(LightInput,
		EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(HeavyInput,
			EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(LightInput,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_GroundedMeleeAttack::LightComboSavedTagChanged);

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(HeavyInput,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_GroundedMeleeAttack::HeavyComboSavedTagChanged);
 
}
bool UGA_GroundedMeleeAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_GroundedMeleeAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_GroundedMeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}

void UGA_GroundedMeleeAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_GroundedMeleeAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ResetGroundedMeleeAttack();
}

void UGA_GroundedMeleeAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{

}
