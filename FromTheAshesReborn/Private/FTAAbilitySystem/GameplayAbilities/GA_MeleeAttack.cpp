#include "FTAAbilitySystem/GameplayAbilities/GA_MeleeAttack.h"
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
	UAnimMontage* MontageToPlay;
	if(LightAttacks.IsValidIndex(CurrentComboIndex))
	{
		MontageToPlay = LightAttacks[CurrentComboIndex]->MontageToPlay;
		Task = UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, FGameplayTagContainer(),
		1.0f, NAME_None, false, 1.0f);
		Task->OnBlendOut.AddDynamic(this, &UGA_MeleeAttack::OnCompleted);
		Task->OnCompleted.AddDynamic(this, &UGA_MeleeAttack::OnCompleted);
		Task->OnInterrupted.AddDynamic(this, &UGA_MeleeAttack::OnCancelled);
		Task->OnCancelled.AddDynamic(this, &UGA_MeleeAttack::OnCancelled);
		Task->EventReceived.AddDynamic(this, &UGA_MeleeAttack::EventReceived);
	
		Task->ReadyForActivation();
		WaitForComboWindow();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
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
}

void UGA_MeleeAttack::WaitForComboWindow()
{
	
}

void UGA_MeleeAttack::ProceedToNextCombo()
{
}

void UGA_MeleeAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UGA_MeleeAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UGA_MeleeAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
	if(EventTag == FGameplayTag::RequestGameplayTag(FName("Character.State.Grounded.Attacking.Melee")))
	{
		UE_LOG(LogTemp, Warning, TEXT("OK!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No!"));
	}
}
