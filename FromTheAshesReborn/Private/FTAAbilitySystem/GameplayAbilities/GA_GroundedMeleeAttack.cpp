#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedMeleeAttack.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"
#include "DataAsset/MeleeAttackDataAsset.h"
#include "Player/FTAPlayerController.h"

UGA_GroundedMeleeAttack::UGA_GroundedMeleeAttack()
{
}

void UGA_GroundedMeleeAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_GroundedMeleeAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(LightComboWindow,
		EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(HeavyComboWindow,
			EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	
	CurrentComboIndex = 0;
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(LightInput);
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(HeavyInput);
	ComboTagContainer.Reset();
	GetWorld()->GetTimerManager().ClearTimer(FLightComboWindowTimer);
	GetWorld()->GetTimerManager().ClearTimer(FHeavyComboWindowTimer);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_GroundedMeleeAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{

}

void UGA_GroundedMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
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

void UGA_GroundedMeleeAttack::ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(LightComboWindow))
	{
		GetWorld()->GetTimerManager().SetTimer(FLightComboWindowTimer, this, &UGA_GroundedMeleeAttack::LightComboWindowOpen, 0.1f, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(FLightComboWindowTimer);
	}
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(HeavyComboWindow))
	{
		GetWorld()->GetTimerManager().SetTimer(FHeavyComboWindowTimer, this, &UGA_GroundedMeleeAttack::HeavyComboWindowOpen, 0.1f, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(FHeavyComboWindowTimer);
	}
}

void UGA_GroundedMeleeAttack::ProccedToLightCombo()
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponentFromActorInfo()->GetActivatableAbilities())
	{
		if (Spec.Ability)
		{
			if(Spec.InputID == 7)
			{
				GetAbilitySystemComponentFromActorInfo()->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void UGA_GroundedMeleeAttack::ProccedToHeavyCombo()
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponentFromActorInfo()->GetActivatableAbilities())
	{
		if (Spec.Ability)
		{
			if(Spec.InputID == 8)
			{
				
				//ActivateAbility(Spec.Handle)
				if(Spec.Handle.IsValid())
				{
					UE_LOG(LogTemp, Warning, TEXT("VALID"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("NOT VALID"));
				}
				if(Spec.IsActive())
				{
					UE_LOG(LogTemp, Warning, TEXT("Ability is active"));
				}
				GetAbilitySystemComponentFromActorInfo()->TryActivateAbility(Spec.Handle);
			}
		}
	}
}


void UGA_GroundedMeleeAttack::LightComboWindowOpen()
{
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(LightInput))
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(LightInput);
		CurrentComboIndex += 1;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		ProccedToLightCombo();
	}
}

void UGA_GroundedMeleeAttack::HeavyComboWindowOpen()
{
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(HeavyInput))
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(HeavyInput);
		CurrentComboIndex += 1;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		ProccedToHeavyCombo();
	}
}

void UGA_GroundedMeleeAttack::LightComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(LightComboWindow))
	{
		GetWorld()->GetTimerManager().SetTimer(FLightComboWindowTimer, this, &UGA_GroundedMeleeAttack::LightComboWindowOpen, 0.1f, true);
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
		GetWorld()->GetTimerManager().SetTimer(FHeavyComboWindowTimer, this, &UGA_GroundedMeleeAttack::HeavyComboWindowOpen, 0.1f, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(FHeavyComboWindowTimer);
	}
}
void UGA_GroundedMeleeAttack::PerformGroundedMeleeAttack(TArray<TObjectPtr<UMeleeAttackDataAsset>> GroundedAttackDataAssets)
{
	if (!GroundedAttackDataAssets.IsValidIndex(CurrentComboIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("Index out of bounds: %d"), CurrentComboIndex);
		ResetGroundedMeleeAttack();
	}
	
	if (!GroundedAttackDataAssets[CurrentComboIndex])
	{
		UE_LOG(LogTemp, Error, TEXT("No asset at index"));
		ResetGroundedMeleeAttack();
	}
	
	if (ComboTagContainer == GroundedAttackDataAssets[CurrentComboIndex]->RequiredTags)
	{
		if(CurrentComboIndex == GroundedAttackDataAssets[CurrentComboIndex]->RequiredIndex)
		{
			AttackMontageToPlay = GroundedAttackDataAssets[CurrentComboIndex]->MontageToPlay;
			if(AttackMontageToPlay)
			{
				FGameplayTag AttackIndentiferTag = GroundedAttackDataAssets[CurrentComboIndex]->AttackIndentiferTag;
				ComboTagContainer.AddTag(AttackIndentiferTag);

				PlayAttackMontage(AttackMontageToPlay);
				
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AttackMontageToPlay is NULL"));
				ResetGroundedMeleeAttack();

			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No matching index"));
			ResetGroundedMeleeAttack();

		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No matching tag containers"));
		ResetGroundedMeleeAttack();
	}
}

void UGA_GroundedMeleeAttack::ResetGroundedMeleeAttack()
{
	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(LightComboWindow,
		EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(HeavyComboWindow,
			EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	
	CurrentComboIndex = 0;
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(LightInput);
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(HeavyInput);
	ComboTagContainer.Reset();
	GetWorld()->GetTimerManager().ClearTimer(FLightComboWindowTimer);
	GetWorld()->GetTimerManager().ClearTimer(FHeavyComboWindowTimer);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
