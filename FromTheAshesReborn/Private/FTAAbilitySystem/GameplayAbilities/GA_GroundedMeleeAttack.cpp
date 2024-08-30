#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedMeleeAttack.h"

UGA_GroundedMeleeAttack::UGA_GroundedMeleeAttack()
{
	AbilityInputIDs.Add(EAbilityInputID::LightAttack);
	AbilityInputIDs.Add(EAbilityInputID::HeavyAttack);

	CurrentComboIndex = 0;
	IsComboQueued = false;
}

void UGA_GroundedMeleeAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_GroundedMeleeAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_GroundedMeleeAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_GroundedMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(AbilityInputIDs[0] == AbilityInputID)
	{
		UE_LOG(LogTemp, Warning, TEXT("Light"));
	}
	if(AbilityInputIDs[1] == AbilityInputID)
	{
		UE_LOG(LogTemp, Warning, TEXT("Heavy"));
	}
}

bool UGA_GroundedMeleeAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_GroundedMeleeAttack::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
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

void UGA_GroundedMeleeAttack::PlayAttackMontage()
{
}
