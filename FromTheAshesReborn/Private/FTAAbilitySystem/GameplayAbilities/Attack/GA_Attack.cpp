#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_Attack.h"

#include "AbilitySystemGlobals.h"
#include "Camera/CameraSystemComponent.h"
#include "CombatComponents/ComboManagerComponent.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_ReceiveHit.h"
#include "FTAAbilitySystem/GameplayCues/HitCueObject.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
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

	AActor* TargetActor = LastItem.GetActor();

	if (TargetActor && TargetActor->Implements<UAbilitySystemInterface>())
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(TargetActor);
		UAbilitySystemComponent* TargetASC = AbilitySystemInterface->GetAbilitySystemComponent();

		//TODO: Temporary, change later
		AEnemyBaseCharacter* Enemy = Cast<AEnemyBaseCharacter>(Cast<AEnemyBaseCharacter>(GetFTACharacterFromActorInfo()));

		if(Enemy)
		{
			AEnemyBaseCharacter* TargetEnemy = Cast<AEnemyBaseCharacter>(TargetActor);
			if(TargetEnemy)
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
				return;
			}
		}

		if (TargetASC)
		{
			FGameplayAbilityTargetDataHandle TargetHitDataHandle = AddHitResultToTargetData(LastItem);
			if(TargetHitDataHandle.Num() > 0 && TargetHitDataHandle.Get(0))
			{
				
				ExecuteHitLogic(TargetHitDataHandle);
			}
		}
	}
}

FGameplayAbilityTargetDataHandle UGA_Attack::AddHitResultToTargetData(const FHitResult& LastItem)
{
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(LastItem);

	if(!TargetData)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Target Data"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return nullptr;
	}
	
	TargetData->HitResult = LastItem;
	
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	TargetDataHandle.Add(TargetData);
	return TargetDataHandle;
}

void UGA_Attack::ExecuteHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
	UAbilitySystemComponent* TargetASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);

	const FGameplayAbilityActorInfo* TargetActorInfo = TargetASC->AbilityActorInfo.Get();
	// UE_LOG(LogTemp, Warning, TEXT("Size: %d"), CurrentAttackData.PossibleHitReactions.Num());
	
	for(TSubclassOf HitAbilityClass : CurrentAttackData.PossibleHitReactions)
	{
		if(HitAbilityClass && HitAbilityClass->IsValidLowLevel())
		{
			const UGA_ReceiveHit* const CDO = HitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
			if (CDO)
			{
				
				GrantHitAbility(TargetDataHandle, HitAbilityClass);
					UE_LOG(LogTemp, Warning, TEXT("Here 75"))
				
				const FGameplayAbilitySpec* TargetSpec = TargetASC->FindAbilitySpecFromClass(HitAbilityClass);
				if(CDO->CanActivateAbility(TargetSpec->Handle, TargetActorInfo, nullptr, nullptr, nullptr))
				{
					// ApplyMeleeHitEffects(TargetDataHandle, HitAbilityClass);
					// SendMeleeHitGameplayEvents(TargetDataHandle, HitAbilityClass);
					// AddMeleeHitCues(TargetDataHandle, HitAbilityClass);
					UE_LOG(LogTemp, Warning, TEXT("Here 23"))
					break;
				}
				else
				{
					
				}
			}
		}
	}
}

void UGA_Attack::GrantHitAbility(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	TSubclassOf<UGA_ReceiveHit> InHitAbilityClass)
{
	if (InHitAbilityClass->IsValidLowLevel())
	{
		const UGA_ReceiveHit* const CDO = InHitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
		if (CDO)
		{
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CDO->ReceiveHitTag))
			{
				const TSubclassOf<UGameplayEffect>* GrantAbilityEffect = CDO->ReceiveHitEffectMap.Find(CDO->ReceiveHitTag);

				if(GrantAbilityEffect)
				{
					FGameplayEffectSpecHandle GrantAbilityEffectHandle = MakeOutgoingGameplayEffectSpec(*GrantAbilityEffect, 1.0f);

					TArray<FActiveGameplayEffectHandle> AppliedHitEffects = ApplyGameplayEffectSpecToTarget(
							CurrentSpecHandle,
							CurrentActorInfo,
							CurrentActivationInfo,
							GrantAbilityEffectHandle,
							TargetDataHandle
						);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::GrantHitAbility - GrantAbilityEffect is null"))
					EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
					return;
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::GrantHitAbility - Invalid tag"))
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
				return;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::GrantHitAbility - Invalid CDO"))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::GrantHitAbility - InHitAbilityClass is invalid"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
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


	

