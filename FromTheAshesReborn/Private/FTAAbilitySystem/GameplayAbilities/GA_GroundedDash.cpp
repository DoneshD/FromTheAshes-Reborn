#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedDash.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"
#include "DataAsset/DashDataAsset.h"

UGA_GroundedDash::UGA_GroundedDash()
{
}

void UGA_GroundedDash::ResetDash()
{
}

void UGA_GroundedDash::DashWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
}

void UGA_GroundedDash::DashWindowTagOpen()
{
}

void UGA_GroundedDash::PerformDash(TArray<TObjectPtr<UDashDataAsset>> SelectDashDataAssets)
{
}


void UGA_GroundedDash::PlayDashMontage(TObjectPtr<UAnimMontage> DashMontage)
{
}

void UGA_GroundedDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool UGA_GroundedDash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_GroundedDash::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_GroundedDash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_GroundedDash::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UGA_GroundedDash::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UGA_GroundedDash::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
}
