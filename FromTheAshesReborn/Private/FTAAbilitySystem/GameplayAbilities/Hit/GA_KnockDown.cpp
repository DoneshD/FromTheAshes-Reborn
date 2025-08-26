#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_KnockDown.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "DataAsset/HitReactionDataAsset.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"

UGA_KnockDown::UGA_KnockDown()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag("HitTag.Effect.GrantAbility.Knockdown");
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::OwnedTagAdded;
	AbilityTriggers.Add(TriggerData);
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
}

void UGA_KnockDown::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_KnockDown::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}

void UGA_KnockDown::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_KnockDown::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);

}

void UGA_KnockDown::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}

void UGA_KnockDown::OnMontageBlendingOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageBlendingOut(EventTag, EventData);

	// FGameplayEffectContextHandle ContextHandle;
	// FGameplayEffectSpecHandle SpecHandle = GetFTAAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(EnableDownedCombatEffect, 1, ContextHandle);
	//
	// FActiveGameplayEffectHandle AppliedEffects = ApplyGameplayEffectSpecToOwner(
	// 	CurrentSpecHandle,
	// 	CurrentActorInfo,
	// 	CurrentActivationInfo,
	// 	SpecHandle);

	FGameplayEffectSpecHandle DownedEffectHandle = MakeOutgoingGameplayEffectSpec(EnableDownedCombatEffect, 1.0f);
			
	FActiveGameplayEffectHandle AppliedDownedEffects = ApplyGameplayEffectSpecToOwner(
			CurrentSpecHandle,
			CurrentActorInfo,
			CurrentActivationInfo,
			DownedEffectHandle
		);

	
	// FGameplayEventData RecoverEventData;
	//
	// RecoverEventData.Instigator = GetAvatarActorFromActorInfo();
	// RecoverEventData.Target = GetAvatarActorFromActorInfo();
	//
	// if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(RecoveryTag))
	// {
	// 	RecoverEventData.EventTag = RecoveryTag;
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("UGA_KnockDown::OnMontageBlendingOut - RecoveryTag is NULL"));
	// }
		
	// UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), RecoverEventData.EventTag, RecoverEventData);
}
