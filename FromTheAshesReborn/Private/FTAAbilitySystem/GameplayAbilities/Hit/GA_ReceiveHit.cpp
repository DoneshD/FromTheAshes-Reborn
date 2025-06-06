#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_ReceiveHit.h"

#include "DataAsset/HitReactionDataAsset.h"
#include "EventObjects/HitEventObject.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

UGA_ReceiveHit::UGA_ReceiveHit()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag("TestTag.Tag6");
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_ReceiveHit::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
}

bool UGA_ReceiveHit::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_ReceiveHit::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// if(!CurrentEventData.OptionalObject)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("UGA_ReceiveHit::ActivateAbility - CurrentEventData.OptionalObject is Null"));
	// 	
	// }
	//
	// const UHitEventObject* HitInfoObject = Cast<UHitEventObject>(CurrentEventData.OptionalObject);
	//
	// if(!HitInfoObject)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("UGA_ReceiveHit::ActivateAbility - HitInfoObject is Null"));
	// 	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
	// 	return;
	// }
	//
	// if(HitInfoObject->HitData.Instigator)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Actor name: %s: "), *HitInfoObject->HitData.Instigator->GetName());
	// }

	if(HitAbilityAsset)
	{
		if(HitAbilityAsset->MontageToPlay)
		{
			PlayAbilityAnimMontage(HitAbilityAsset->MontageToPlay);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MontageToPlay null"))
	
		}
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("HitAbilityAsset null"))
	}

	
}

void UGA_ReceiveHit::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_ReceiveHit::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_ReceiveHit::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_ReceiveHit::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

}

void UGA_ReceiveHit::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}
