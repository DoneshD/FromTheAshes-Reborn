#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_Attack.h"

#include "Camera/CameraSystemComponent.h"
#include "CombatComponents/ComboManagerComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_ReceiveHit.h"
#include "FTAAbilitySystem/GameplayCues/HitCueObject.h"
#include "FTACustomBase/FTACharacter.h"
#include "TracingComponent/TracingComponent.h"
#include "Weapon/EquipmentManagerComponent.h"
#include "Weapon/WeaponActorBase.h"

UGA_Attack::UGA_Attack(const FObjectInitializer&)
{
}

void UGA_Attack::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
}

bool UGA_Attack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	
}
void UGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if(!GetFTACharacterFromActorInfo()->EquipmentManagerComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - EquipmentManagerComponent is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	WeaponActors = GetFTACharacterFromActorInfo()->EquipmentManagerComponent->GetEquippedWeaponActors();
	
	for (AWeaponActorBase* SpawnedActor : GetFTACharacterFromActorInfo()->EquipmentManagerComponent->GetEquippedWeaponActors())
	{
		if(SpawnedActor)
		{
			if(SpawnedActor->SkeletalMesh)
			{
				WeaponActors.Add(SpawnedActor);
			}
		}
	}
	
	if(WeaponActors.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - MeleeWeaponActor is empty"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	// GetFTAAbilitySystemComponentFromActorInfo()->OnAbilityRuntimeData.AddUniqueDynamic(this, &UGA_Attack::SetAbilityRuntimeData);

	WeaponActors[0]->TracingComponent->OnItemAdded.AddDynamic(this, &UGA_Attack::OnHitAdded);
	WeaponActors[0]->TracingComponent->BoxHalfSize = FVector(
					100,
					100,
					100);
	
	// for (AWeaponActorBase* WeaponActor : WeaponActors)
	// {
	// 	if(WeaponActor)
	// 	{
	// 		if(WeaponActor->TracingComponent)
	// 		{
	// 			WeaponActor->TracingComponent->OnItemAdded.AddDynamic(this, &UGA_Attack::OnHitAdded);
	// 			UE_LOG(LogTemp, Warning, TEXT("Hit binded"))
	//
	// 			//Refactor for ranged
	// 			WeaponActor->TracingComponent->BoxHalfSize = FVector(
	// 				100,
	// 				100,
	// 				100);
	// 			
	// 		}
	// 	}
	// }
}

void UGA_Attack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Attack::OnHitAdded(FHitResult LastItem)
{
	UE_LOG(LogTemp, Warning, TEXT("Attack landed"));
}

UFTAAbilityDataAsset* UGA_Attack::SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets)
{
	return Super::SelectAbilityAsset(InAbilityAssets);
}

void UGA_Attack::ExtractAssetProperties(UFTAAbilityDataAsset* InAbilityAsset)
{
	Super::ExtractAssetProperties(InAbilityAsset);
	UAttackAbilityDataAsset* AttackAsset = Cast<UAttackAbilityDataAsset>(InAbilityAsset);

	if(!AttackAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("AttackAsset is null"));
		return;
	}
	
	//Damage
	if(AttackAsset->AttackData.ApplyDamageEffect && AttackAsset->AttackData.ApplyDamageEffect->IsValidLowLevel())
	{
		CurrentAttackData.ApplyDamageEffect = AttackAsset->AttackData.ApplyDamageEffect;
	}
	
	//Hit Reactions
	if(AttackAsset->AttackData.PossibleHitReactions.Num() > 0 && !AttackAsset->AttackData.PossibleHitReactions.IsEmpty())
	{
		for (TSubclassOf HitReaction : AttackAsset->AttackData.PossibleHitReactions)
		{
			if(HitReaction && HitReaction->IsValidLowLevel())
			{
				if (!CurrentAttackData.PossibleHitReactions.Contains(HitReaction))
				{
					CurrentAttackData.PossibleHitReactions.Insert(HitReaction, 0);
				}
			}
			
		}
	}
	
	//Hit Cue
	if(AttackAsset->AttackData.HitCueClass && AttackAsset->AttackData.HitCueClass->IsValidLowLevel())
	{
		CurrentAttackData.HitCueClass = AttackAsset->AttackData.HitCueClass;
	}
}

void UGA_Attack::PerformAbility(UFTAAbilityDataAsset* AttackTypes)
{
	Super::PerformAbility(AttackTypes);
	
}

void UGA_Attack::ResetAttack()
{
	
}

void UGA_Attack::PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage)
{
	Super::PlayAbilityAnimMontage(AnimMontage);
}

void UGA_Attack::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_Attack::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);
}

void UGA_Attack::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}


	

