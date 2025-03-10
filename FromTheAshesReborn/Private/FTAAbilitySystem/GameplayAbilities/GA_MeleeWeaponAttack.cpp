﻿#include "FTAAbilitySystem/GameplayAbilities/GA_MeleeWeaponAttack.h"

#include "AbilitySystemComponent.h"
#include "DidItHitActorComponent.h"
#include "DataAsset/MeleeAttackDataAsset.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"
#include "Player/PlayerComboManagerInterface.h"
#include "Weapon/MeleeWeaponInstance.h"
#include "Weapon/WeaponActorBase.h"

UGA_MeleeWeaponAttack::UGA_MeleeWeaponAttack(const FObjectInitializer&)
{
}

UMeleeWeaponInstance* UGA_MeleeWeaponAttack::GetMeleeWeaponInstance() const
{
	return Cast<UMeleeWeaponInstance>(GetAssociatedWeaponInstance());
}

bool UGA_MeleeWeaponAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_MeleeWeaponAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	UMeleeWeaponInstance* WeaponData = GetMeleeWeaponInstance();
	
	for (AActor* SpawnedActor : WeaponData->GetSpawnedActors())
	{
		//Using interface to call toggle function
		AWeaponActorBase* WeaponActor = Cast<AWeaponActorBase>(SpawnedActor);
		if(WeaponActor)
		{
			// MeleeWeaponActor = WeaponActor;
			// MeleeWeaponActor->DidItHitActorComponent->OnItemAdded.AddDynamic(this, &UGA_MeleeWeaponAttack::OnHitAdded);
			//Need to try to bind with a function that takes a ref
			GetFTAAbilitySystemComponentFromActorInfo()->TestWeaponActor->DidItHitActorComponent->OnItemAdded.AddDynamic(this, &UGA_MeleeWeaponAttack::OnHitAdded);
			PerformMeleeAttack(MeleeAttackAssets);
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NOT AWeaponActorBase"));
		}
	}
	
}

void UGA_MeleeWeaponAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_MeleeWeaponAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	GetFTAAbilitySystemComponentFromActorInfo()->TestWeaponActor->DidItHitActorComponent->OnItemAdded.RemoveAll(this);
}

void UGA_MeleeWeaponAttack::ResetMeleeAttack()
{
	IPlayerComboManagerInterface* PlayerComboManagerInterface = Cast<IPlayerComboManagerInterface>(GetAvatarActorFromActorInfo());

	PlayerComboManagerInterface->GetCurrentComboContainer().Reset();
	PlayerComboManagerInterface->SetCurrentComboIndex(0);
}
bool UGA_MeleeWeaponAttack::FindMatchingTagContainer(const TArray<UMeleeAttackDataAsset*>& MeleeAttackDataAssets, TObjectPtr<UMeleeAttackDataAsset>& OutMatchingDataAsset)
{
	IPlayerComboManagerInterface* PlayerComboManagerInterface = Cast<IPlayerComboManagerInterface>(GetAvatarActorFromActorInfo());
	
	for (int32 Index = 0; Index < MeleeAttackDataAssets.Num(); ++Index)
	{
		if (MeleeAttackDataAssets[Index])
		{
			if (PlayerComboManagerInterface->GetCurrentComboContainer().HasAll(MeleeAttackDataAssets[Index]->RequiredTags))
			{
				if(MeleeAttackDataAssets[Index]->RequiredIndex == PlayerComboManagerInterface->GetCurrentComboIndex())
				{
					OutMatchingDataAsset = MeleeAttackDataAssets[Index];
					return true;
				}
			}
		}
	}
	PlayerComboManagerInterface->GetCurrentComboContainer().Reset();
	PlayerComboManagerInterface->SetCurrentComboIndex(0);
	OutMatchingDataAsset = MeleeAttackDataAssets[0];
	return true;
}

void UGA_MeleeWeaponAttack::PerformMeleeAttack(TArray<UMeleeAttackDataAsset*> MeleeAttackDataAssets)
{
	IPlayerComboManagerInterface* PlayerComboManagerInterface = Cast<IPlayerComboManagerInterface>(GetAvatarActorFromActorInfo());

	TObjectPtr<UMeleeAttackDataAsset> MatchingDataAsset;
	bool DataAssetFound = FindMatchingTagContainer(MeleeAttackDataAssets, MatchingDataAsset);
	
	if(!DataAssetFound)
	{
		UE_LOG(LogTemp, Error, TEXT("DA NOT found"));
		ResetMeleeAttack();
		return;
	}
	
	if(!MatchingDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("MatchingDataAsset is NULL"));
		ResetMeleeAttack();
		return;
	}

	AttackMontageToPlay = MatchingDataAsset->MontageToPlay;
	
	if (AttackMontageToPlay)
	{
		FGameplayTag AttackIndentiferTag = MatchingDataAsset->AttackIndentiferTag;
		int32 CurrentComboIndex = PlayerComboManagerInterface->GetCurrentComboIndex();
    
		PlayerComboManagerInterface->GetCurrentComboContainer().AddTag(AttackIndentiferTag);
		PlayerComboManagerInterface->SetCurrentComboIndex(CurrentComboIndex + 1);

		PlayAttackMontage(AttackMontageToPlay);
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("AttackMontageToPlay is NULL"));
		ResetMeleeAttack();
	}
}

void UGA_MeleeWeaponAttack::PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage)
{
	AttackMontageToPlay = AttackMontage;
	if(AttackMontageToPlay)
	{
		Task = UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, AttackMontageToPlay, FGameplayTagContainer(),
		1.0f, NAME_None, false, 1.0f);
		Task->OnBlendOut.AddDynamic(this, &UGA_MeleeWeaponAttack::OnCompleted);
		Task->OnCompleted.AddDynamic(this, &UGA_MeleeWeaponAttack::OnCompleted);
		Task->OnInterrupted.AddDynamic(this, &UGA_MeleeWeaponAttack::OnCancelled);
		Task->OnCancelled.AddDynamic(this, &UGA_MeleeWeaponAttack::OnCancelled);
		Task->EventReceived.AddDynamic(this, &UGA_MeleeWeaponAttack::EventReceived);
		
		Task->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No attack montage"));
	}
}


void UGA_MeleeWeaponAttack::StartMeleeWeaponTargeting()
{
	check(CurrentActorInfo);

	if(!GetFTAAbilitySystemComponentFromActorInfo()->TestWeaponActor)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::StartMeleeWeaponTargeting - No Melee Weapon Actor"))
		return;
	}
	
	GetFTAAbilitySystemComponentFromActorInfo()->TestWeaponActor->DidItHitActorComponent->ToggleTraceCheck(true);
}

void UGA_MeleeWeaponAttack::EndMeleeWeaponTargeting()
{
	if(!GetFTAAbilitySystemComponentFromActorInfo()->TestWeaponActor)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::EndMeleeWeaponTargeting - No Melee Weapon Actor"))
		return;
	}
	GetFTAAbilitySystemComponentFromActorInfo()->TestWeaponActor->DidItHitActorComponent->ToggleTraceCheck(false);
	GetFTAAbilitySystemComponentFromActorInfo()->TestWeaponActor->DidItHitActorComponent->ClearHitArray();
}

void UGA_MeleeWeaponAttack::OnMeleeWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData)
{
	ApplyGameplayEffectToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, TargetData, MeleeAttackDamageEffect, 1, 1);
}

void UGA_MeleeWeaponAttack::OnHitAdded(FHitResult LastItem)
{
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(LastItem);
	TargetData->HitResult = LastItem;
	if(!TargetData)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Target Data"))
		return;
	}
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	TargetDataHandle.Add(TargetData);
	
	OnMeleeWeaponTargetDataReady(TargetDataHandle);
}

void UGA_MeleeWeaponAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UGA_MeleeWeaponAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ResetMeleeAttack();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_MeleeWeaponAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
}
