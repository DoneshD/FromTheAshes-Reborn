#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_Launched.h"

#include "EventObjects/LaunchEventObject.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/AT_LaunchCharacterAndWait.h"
#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack.h"

UGA_Launched::UGA_Launched()
{
}

void UGA_Launched::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
}

bool UGA_Launched::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Launched::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const ULaunchEventObject* LaunchInfoObject = Cast<ULaunchEventObject>(CurrentEventData.OptionalObject);
	
	if(!LaunchInfoObject)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_Launched::ActivateAbility - LaunchInfoObject is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	LaunchTask = UAT_LaunchCharacterAndWait::AT_LaunchCharacterAndWait(
		this,
		LaunchInfoObject->LaunchData.VerticalDistance,
		LaunchInfoObject->LaunchData.LaunchDuration,
		LaunchInfoObject->LaunchData.StallDuration,
		LaunchOffset);
	
	if (LaunchTask)
	{
		LaunchTask->OnLaunchComplete.AddDynamic(this, &UGA_Launched::OnLaunchComplete);
		LaunchTask->ReadyForActivation();
	}
	
}

void UGA_Launched::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Launched::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	GetFTAAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Flail"));

	if(EnableAerialCombatEffect)
	{
		FGameplayEffectSpecHandle GEHandle = MakeOutgoingGameplayEffectSpec(EnableAerialCombatEffect, 1.0f);
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
	}
	
	if (GetFTAAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(HitReactionTag))
	{
		GetFTAAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(HitReactionTag));
	}
	
}

void UGA_Launched::OnLaunchComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

}
