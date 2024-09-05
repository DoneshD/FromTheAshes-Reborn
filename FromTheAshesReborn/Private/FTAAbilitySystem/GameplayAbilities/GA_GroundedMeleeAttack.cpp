#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedMeleeAttack.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"
#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedLightMeleeAttack.h"

UGA_GroundedMeleeAttack::UGA_GroundedMeleeAttack()
{
	CurrentComboIndex = 0;
	//IsComboQueued = false;

}

void UGA_GroundedMeleeAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_GroundedMeleeAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	CurrentComboIndex = 0;
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Light.Saved")));
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Heavy.Saved")));

	ComboTagContainer.Reset();

}

void UGA_GroundedMeleeAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogTemp, Warning, TEXT("UGA_GroundedMeleeAttack Event receieved"));

}

void UGA_GroundedMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Event.Montage.Combo.Light.Open")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_GroundedMeleeAttack::ComboLightWindowTagChanged);
	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Event.Montage.Combo.Heavy.Open")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_GroundedMeleeAttack::ComboHeavyWindowTagChanged);

}

bool UGA_GroundedMeleeAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_GroundedMeleeAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	UE_LOG(LogTemp, Warning, TEXT("Finally free"));
}

void UGA_GroundedMeleeAttack::ProceedToLightCombo()
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

void UGA_GroundedMeleeAttack::ProceedToHeavyCombo()
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponentFromActorInfo()->GetActivatableAbilities())
	{
		if (Spec.Ability)
		{
			if(Spec.InputID == 8)
			{
				GetAbilitySystemComponentFromActorInfo()->TryActivateAbility(Spec.Handle);

			}
		}
	}
}

void UGA_GroundedMeleeAttack::CheckForLightInput()
{

	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Light.Saved"))))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		CurrentComboIndex += 1;
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Light.Saved")));


		ProceedToLightCombo();
	}
}

void UGA_GroundedMeleeAttack::CheckForHeavyInput()
{
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Heavy.Saved"))))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		CurrentComboIndex += 1;
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Heavy.Saved")));


		ProceedToHeavyCombo();
	}
}

void UGA_GroundedMeleeAttack::PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage)
{
	AttackMontageToPlay = AttackMontage;
	Task = UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, AttackMontageToPlay, FGameplayTagContainer(),
	1.0f, NAME_None, false, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UGA_GroundedMeleeAttack::EventReceived);
	
	Task->ReadyForActivation();
}

void UGA_GroundedMeleeAttack::ComboLightWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Montage.Combo.Light.Open"))))
	{
		GetWorld()->GetTimerManager().SetTimer(FComboWindowTimer, this, &UGA_GroundedMeleeAttack::CheckForLightInput, 0.1f, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(FComboWindowTimer);
	}
}

void UGA_GroundedMeleeAttack::ComboHeavyWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Montage.Combo.Heavy.Open"))))
	{
		GetWorld()->GetTimerManager().SetTimer(FComboWindowTimer, this, &UGA_GroundedMeleeAttack::CheckForHeavyInput, 0.1f, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(FComboWindowTimer);
	}
}


