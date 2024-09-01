#include "FTAAbilitySystem/GameplayAbilities/GA_MeleeAttack.h"

#include "AbilitySystemComponent.h"
#include "DataAsset/MeleeAttackDataAsset.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"

UGA_MeleeAttack::UGA_MeleeAttack()
{
	CurrentComboIndex = 0;
	MaxComboNum = 7;
	IsComboQueued = false;
}

void UGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// MontageToPlay = LightAttacks[CurrentComboIndex]->MontageToPlay;
	// Task = UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, FGameplayTagContainer(),
	// 1.0f, NAME_None, false, 1.0f);
	// Task->OnBlendOut.AddDynamic(this, &UGA_MeleeAttack::OnCompleted);
	// Task->OnCompleted.AddDynamic(this, &UGA_MeleeAttack::OnCompleted);
	// Task->OnInterrupted.AddDynamic(this, &UGA_MeleeAttack::OnCancelled);
	// Task->OnCancelled.AddDynamic(this, &UGA_MeleeAttack::OnCancelled);
	// Task->EventReceived.AddDynamic(this, &UGA_MeleeAttack::EventReceived);
	
	// Task->ReadyForActivation();
	// if(LightAttacks.IsValidIndex(CurrentComboIndex))
	// {
	// 	PlayAttackMontage();
	// }
	// else
	// {
	// 	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	// }
	//PlayAttackMontage();
}

bool UGA_MeleeAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_MeleeAttack::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

void UGA_MeleeAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_MeleeAttack::OnComboWindowOpen(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
}

void UGA_MeleeAttack::OnComboWindowClose(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{

	// End ability if no combo input was registered
	if (!IsComboQueued)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		// Proceed to the next combo stage
		ProceedToNextCombo();
	}
}

void UGA_MeleeAttack::WaitForComboWindow()
{
	
}

void UGA_MeleeAttack::ProceedToNextCombo()
{
	CurrentComboIndex = (CurrentComboIndex + 1) % MaxComboNum;

	if(LightAttacks.IsValidIndex(CurrentComboIndex))
	{
		PlayAttackMontage();
	}
	else
	{
		CurrentComboIndex = 0;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_MeleeAttack::CheckForInput()
{
	UE_LOG(LogTemp, Warning, TEXT("Checking"));
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Light.Saved"))))
	{
		UE_LOG(LogTemp, Warning, TEXT("NEXT combo"));
		IsComboQueued = true;
		ProceedToNextCombo();
	}
}

void UGA_MeleeAttack::PlayAttackMontage()
{
	// MontageToPlay = LightAttacks[CurrentComboIndex]->MontageToPlay;
	// Task = UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, FGameplayTagContainer(),
	// 1.0f, NAME_None, false, 1.0f);
	// Task->OnBlendOut.AddDynamic(this, &UGA_MeleeAttack::OnCompleted);
	// Task->OnCompleted.AddDynamic(this, &UGA_MeleeAttack::OnCompleted);
	// Task->OnInterrupted.AddDynamic(this, &UGA_MeleeAttack::OnCancelled);
	// Task->OnCancelled.AddDynamic(this, &UGA_MeleeAttack::OnCancelled);
	// Task->EventReceived.AddDynamic(this, &UGA_MeleeAttack::EventReceived);
	//
	// Task->ReadyForActivation();
	WaitForComboWindow();
}

void UGA_MeleeAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogTemp, Warning, TEXT("UGA_MeleeAttack::OnCancelled"));
	// if(!IsComboQueued) 
	// {
	// 	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	// }
}

void UGA_MeleeAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_MeleeAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogTemp, Warning, TEXT("EventReceived"));

	if(EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.Combo.Light.Open")))
	{
		GetWorld()->GetTimerManager().SetTimer(FComboWindowTimer, this, &UGA_MeleeAttack::CheckForInput, 0.1f, true);
	}
	else if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.Combo.Light.Closed")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Closed"));

		GetWorld()->GetTimerManager().ClearTimer(FComboWindowTimer);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No!"));
	}
}
