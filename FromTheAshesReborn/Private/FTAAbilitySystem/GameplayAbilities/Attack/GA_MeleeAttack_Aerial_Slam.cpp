#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeAttack_Aerial_Slam.h"

#include "CombatComponents/ComboManagerComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"

UGA_MeleeAttack_Aerial_Slam::UGA_MeleeAttack_Aerial_Slam(const FObjectInitializer&)
{
}

bool UGA_MeleeAttack_Aerial_Slam::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_MeleeAttack_Aerial_Slam::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if(!ComboManagerComponent)
	{
		ComboManagerComponent = GetFTACharacterFromActorInfo()->ComboManagerComponent;
	}
	
	ResetCombo();
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(!GetFTAAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Character.DisableRotation")))
	{
		GetFTAAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("Character.DisableRotation"));
	}
}

void UGA_MeleeAttack_Aerial_Slam::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
}

void UGA_MeleeAttack_Aerial_Slam::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	
	if(GetFTAAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Character.DisableRotation")))
	{
		GetFTAAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("Character.DisableRotation"));
	}
}

void UGA_MeleeAttack_Aerial_Slam::PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage)
{
	Super::PlayAbilityAnimMontage(AnimMontage);
}

void UGA_MeleeAttack_Aerial_Slam::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_MeleeAttack_Aerial_Slam::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);
}

void UGA_MeleeAttack_Aerial_Slam::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("MovementTag.Event.ActivateTask")))
	{
		GetFTAAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(LoopTag);
	}	
}

UFTAAbilityDataAsset* UGA_MeleeAttack_Aerial_Slam::SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets)
{
	return Super::SelectAbilityAsset(InAbilityAssets);
}

void UGA_MeleeAttack_Aerial_Slam::ExtractAssetProperties(UFTAAbilityDataAsset* InAbilityAsset)
{
	Super::ExtractAssetProperties(InAbilityAsset);
}

void UGA_MeleeAttack_Aerial_Slam::SetRuntimeAbilityData(UFTAAbilityDataAsset* InAbilityRuntimeData)
{
	Super::SetRuntimeAbilityData(InAbilityRuntimeData);
}

void UGA_MeleeAttack_Aerial_Slam::PerformAbility(UFTAAbilityDataAsset* InAbilityAsset)
{
	Super::PerformAbility(InAbilityAsset);
}

void UGA_MeleeAttack_Aerial_Slam::OnHitAdded(FHitResult LastItem)
{
	Super::OnHitAdded(LastItem);
}

void UGA_MeleeAttack_Aerial_Slam::OnMoveComplete()
{
	Super::OnMoveComplete();

	GetFTAAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(LoopTag);
	ComboManagerComponent->SetCurrentComboIndex(ComboManagerComponent->GetCurrentComboIndex() + 1);

	CurrentAbilityAsset = SelectAbilityAsset(AbilityAssets);
	
	if(!CurrentAbilityAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("SLAM::OnMoveComplete - CurrentAbilityAsset is Null"))
		return;
	}
	
	ExtractAssetProperties(CurrentAbilityAsset);
	PerformAbility(CurrentAbilityAsset);
}
