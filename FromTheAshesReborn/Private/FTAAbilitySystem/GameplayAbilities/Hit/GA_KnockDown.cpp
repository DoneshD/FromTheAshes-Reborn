#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_KnockDown.h"
#include "DataAsset/HitReactionDataAsset.h"

UGA_KnockDown::UGA_KnockDown()
{
}

void UGA_KnockDown::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
}

bool UGA_KnockDown::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_KnockDown::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("UGA_KnockDown Active Ability"));


	if(KnockDownAbilityAsset)
	{
		if(KnockDownAbilityAsset->MontageToPlay)
		{
			PlayAbilityAnimMontage(KnockDownAbilityAsset->MontageToPlay);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MontageToPlay null"))
	
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("down null"))
	}
}

void UGA_KnockDown::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_KnockDown::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UE_LOG(LogTemp, Warning, TEXT("End down"))

	
}

void UGA_KnockDown::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_KnockDown::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);

	UE_LOG(LogTemp, Warning, TEXT("Apply"))
	
	FActiveGameplayEffectHandle AppliedDmgEffects = ApplyGameplayEffectToOwner(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		GetUpAbilityEffect.GetDefaultObject(),
		1,
		1
	);
	
}

void UGA_KnockDown::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}
