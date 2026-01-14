#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeAttack.h"

#include "CombatComponents/ComboManagerComponent.h"
#include "DataAsset/MeleeAbilityDataAsset.h"
#include "TracingComponent/TracingComponent.h"
#include "Weapon/WeaponActorBase.h"

UGA_MeleeAttack::UGA_MeleeAttack(const FObjectInitializer&)
{
}

bool UGA_MeleeAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}
void UGA_MeleeAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_MeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	CurrentAttackData.AttackDirectionStruct.AttackDirection = ESpatialDirection::None;
}

void UGA_MeleeAttack::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_MeleeAttack::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);
}

void UGA_MeleeAttack::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.BeginSlash")))
	{
		StartMeleeWeaponTrace();
	}
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.EndSlash")))
	{
		EndMeleeWeaponTrace();
	}
}


UFTAAbilityDataAsset* UGA_MeleeAttack::SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets)
{
	Super::SelectAbilityAsset(InAbilityAssets);
	
	TArray<UFTAAbilityDataAsset*> MeleeAbilityDataAssets;

	for (UFTAAbilityDataAsset* Asset : InAbilityAssets)
	{
		if (UMeleeAbilityDataAsset* MeleeAsset = Cast<UMeleeAbilityDataAsset>(Asset))
		{
			MeleeAbilityDataAssets.Add(MeleeAsset);
		}
	}

	if(TObjectPtr<UFTAAbilityDataAsset> AbilityDataAsset = ComboManagerComponent->GetMeleeAssetByRequirements(MeleeAbilityDataAssets))
	{
		return AbilityDataAsset;
	}
	
	return nullptr;
	
}

void UGA_MeleeAttack::ExtractAssetProperties(UFTAAbilityDataAsset* InAbilityAsset)
{
	Super::ExtractAssetProperties(InAbilityAsset);
	UMeleeAbilityDataAsset* MeleeAbilityDataAsset = Cast<UMeleeAbilityDataAsset>(InAbilityAsset);
	
	if(!MeleeAbilityDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("MeleeAbilityDataAsset is null"));
		return;
	}
}

void UGA_MeleeAttack::PerformAbility(UFTAAbilityDataAsset* InAbilityAsset)
{
	UMeleeAbilityDataAsset* MeleeAbilityDataAsset = Cast<UMeleeAbilityDataAsset>(InAbilityAsset);
	Super::PerformAbility(MeleeAbilityDataAsset);
	
}

void UGA_MeleeAttack::OnHitAdded(FHitResult LastItem)
{
	Super::OnHitAdded(LastItem);
}

void UGA_MeleeAttack::StartMeleeWeaponTrace()
{
	for (AWeaponActorBase* WeaponActor : WeaponActors)
	{
		//Another instance of crashing here
		if(WeaponActor)
		{
			if(WeaponActor->TracingComponent)
			{
				WeaponActor->TracingComponent->BoxHalfSize = FVector(
				100.0f,
				100.0f,
				100.0f);
				
				WeaponActor->TracingComponent->ToggleTraceCheck(true);
			}
		}
	}
}

void UGA_MeleeAttack::EndMeleeWeaponTrace()
{
	for (AWeaponActorBase* WeaponActor : WeaponActors)
	{
		if(WeaponActor)
		{
			if(WeaponActor->TracingComponent)
			{
				WeaponActor->TracingComponent->ToggleTraceCheck(false);
				WeaponActor->TracingComponent->ClearHitArray();
			}
		}
	}
}

void UGA_MeleeAttack::PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage)
{
	Super::PlayAbilityAnimMontage(AnimMontage);
}
