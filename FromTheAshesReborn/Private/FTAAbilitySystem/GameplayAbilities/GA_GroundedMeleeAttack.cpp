#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedMeleeAttack.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"
#include "Interfaces/MeleeCombatantInterface.h"
#include "DataAsset/MeleeAttackDataAsset.h"
#include "Player/FTAPlayerController.h"

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

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(LightComboWindow,
		EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(HeavyComboWindow,
			EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	MeleeCombatantInterface->GetCurrentComboContainer().Reset();
	MeleeCombatantInterface->SetCurrentComboIndex(0);
	
	GetWorld()->GetTimerManager().ClearTimer(FLightComboWindowTimer);
	GetWorld()->GetTimerManager().ClearTimer(FHeavyComboWindowTimer);

	//UE_LOG(LogTemp, Warning, TEXT("Reset Attack"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_GroundedMeleeAttack::LightComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(LightComboWindow))
	{
		GetWorld()->GetTimerManager().SetTimer(FLightComboWindowTimer, this, &UGA_GroundedMeleeAttack::LightComboWindowOpen, 0.01f, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(FLightComboWindowTimer);
	}
}

void UGA_GroundedMeleeAttack::HeavyComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(HeavyComboWindow))
	{
		GetWorld()->GetTimerManager().SetTimer(FHeavyComboWindowTimer, this, &UGA_GroundedMeleeAttack::HeavyComboWindowOpen, 0.01f, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(FHeavyComboWindowTimer);
	}
}

void UGA_GroundedMeleeAttack::LightComboWindowOpen()
{
	if(PC->LastInputSavedTag.MatchesTag(LightInput))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		ProceedToNextCombo(7);
	}
}

void UGA_GroundedMeleeAttack::HeavyComboWindowOpen()
{
	if(PC->LastInputSavedTag.MatchesTag(HeavyInput))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
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
	MeleeCombatantInterface->GetCurrentComboContainer().Reset();
	MeleeCombatantInterface->SetCurrentComboIndex(0);
	OutMatchingDataAsset = GroundedAttackDataAssets[0];
	return true;
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
		PrintCurrentComboContainer();
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

	if(GetAvatarActorFromActorInfo()->GetInstigatorController())
	{
		PC = Cast<AFTAPlayerController>(GetAvatarActorFromActorInfo()->GetInstigatorController());
		if(!PC)
		{
			UE_LOG(LogTemp, Warning, TEXT("!PC"));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("!GetAvatarActorFromActorInfo()->GetInstigatorController()"));
		return;
	}

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(LightComboWindow,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_GroundedMeleeAttack::LightComboWindowTagChanged);

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(HeavyComboWindow,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_GroundedMeleeAttack::HeavyComboWindowTagChanged);
 
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

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(LightComboWindow,
		EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(HeavyComboWindow,
			EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	
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
