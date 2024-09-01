#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedMeleeAttack.h"
#include "DataAsset/MeleeAttackDataAsset.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"

UGA_GroundedMeleeAttack::UGA_GroundedMeleeAttack()
{
	CurrentComboIndex = 0;
	IsComboQueued = false;
}

void UGA_GroundedMeleeAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_GroundedMeleeAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_GroundedMeleeAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogTemp, Warning, TEXT("UGA_GroundedMeleeAttack Event receieved"));

}

void UGA_GroundedMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	AttackMontageToPlay = TESTATTACKS[CurrentComboIndex]->MontageToPlay;
	Task = UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, AttackMontageToPlay, FGameplayTagContainer(),
	1.0f, NAME_None, false, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UGA_GroundedMeleeAttack::EventReceived);
	
	Task->ReadyForActivation();
}

bool UGA_GroundedMeleeAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_GroundedMeleeAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_GroundedMeleeAttack::OnComboWindowOpen(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	
}

void UGA_GroundedMeleeAttack::OnComboWindowClose(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	
}

void UGA_GroundedMeleeAttack::WaitForComboWindow()
{
}

void UGA_GroundedMeleeAttack::ProceedToNextCombo()
{
}

void UGA_GroundedMeleeAttack::CheckForInput()
{
}

void UGA_GroundedMeleeAttack::PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage)
{
	 //  AttackMontageToPlay = AttackMontage;
	 //  AttackMontageToPlay = TESTATTACKS[CurrentComboIndex]->MontageToPlay;
	 //  Task = UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, AttackMontageToPlay, FGameplayTagContainer(),
	 //  1.0f, NAME_None, false, 1.0f);
	 //  Task->OnBlendOut.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCompleted);
	 //  Task->OnCompleted.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCompleted);
	 //  Task->OnInterrupted.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCancelled);
	 //  Task->OnCancelled.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCancelled);
	 //  Task->EventReceived.AddDynamic(this, &UGA_GroundedMeleeAttack::EventReceived);
	 //
	 // Task->ReadyForActivation();
	//WaitForComboWindow();
}
