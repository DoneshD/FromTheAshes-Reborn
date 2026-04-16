#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_Attack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "Camera/CameraSystemComponent.h"
#include "CombatComponents/ComboManagerComponent.h"
#include "DataAsset/MoveToLocationDataAsset.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "EventObjects/HitEventObject.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_ReceiveHit.h"
#include "FTAAbilitySystem/GameplayCues/FTASoundCueObject.h"
#include "FTAAbilitySystem/GameplayCues/FTAVisualCueObject.h"
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
	
	if(WeaponActors[0]->TracingComponent)
	{
		WeaponActors[0]->TracingComponent->OnItemAdded.AddDynamic(this, &UGA_Attack::OnHitAdded);
		WeaponActors[0]->TracingComponent->BoxHalfSize = FVector(
						100,
						100,
						100);
	}
	
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

	for (AWeaponActorBase* WeaponActor : WeaponActors)
	{
		if(!WeaponActor)
		{
			//TODO: Printing incorrectly, fix later
			//UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::EndAbility - GetFTACharacterFromActorInfo()->CurrentWeapon is invalid"))
			return;
		}
		
		WeaponActor->TracingComponent->OnItemAdded.RemoveAll(this);
	}
	
	CurrentAttackData = nullptr;
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

	if(!TargetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetActor null"));
		return;
	}
	UAbilitySystemComponent* TargetASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);

	if(!TargetASC)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetASC null"));
		return;
	}

	const FGameplayAbilityActorInfo* TargetActorInfo = TargetASC->AbilityActorInfo.Get();
	
	if (IsValid(CurrentAttackData) && !CurrentAttackData->PossibleHitReactions.IsEmpty())
	{
		for(FHitDataInfo& HitData : CurrentAttackData->PossibleHitReactions)
		{
			if(HitData.HitAbilityClass && HitData.HitAbilityClass->IsValidLowLevel())
			{
				const UGA_ReceiveHit* const CDO = HitData.HitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
				if (CDO)
				{
					GrantHitAbility(TargetDataHandle, HitData.HitAbilityClass);
					const FGameplayAbilitySpec* TargetSpec = TargetASC->FindAbilitySpecFromClass(HitData.HitAbilityClass);
					if(TargetSpec)
					{
						if(CDO->CanActivateAbility(TargetSpec->Handle, TargetActorInfo, nullptr, nullptr, nullptr))
						{
							ApplyHitEffects(TargetDataHandle, HitData.HitAbilityClass);
							SendHitGameplayEvents(TargetDataHandle, HitData);
							AddHitCues(TargetDataHandle, HitData.HitAbilityClass);
							break;
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("UGA_Attack::ExecuteHitLogic: Invalid spec from class "))
					}
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

void UGA_Attack::ApplyHitEffects(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	TSubclassOf<UGA_ReceiveHit> InHitAbilityClass)
{
	if (InHitAbilityClass->IsValidLowLevel())
	{
		const UGA_ReceiveHit* const CDO = InHitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
		if (CDO)
		{
			if(CurrentAttackData->ApplyDamageEffect)
			{
				TArray<FActiveGameplayEffectHandle> AppliedDamageEffects = ApplyGameplayEffectToTarget(
				CurrentSpecHandle,
				CurrentActorInfo,
				CurrentActivationInfo,
				TargetDataHandle,
				CurrentAttackData->ApplyDamageEffect, 
				1,
				1
				);
			}
			
			/*if(CDO->HitEffect)
			{
				FGameplayEffectSpecHandle HitEffectHandle = MakeOutgoingGameplayEffectSpec(CDO->HitEffect, 1.0f);

				TArray<FActiveGameplayEffectHandle> TestAppliedHitEffects = ApplyGameplayEffectSpecToTarget(
						CurrentSpecHandle,
						CurrentActorInfo,
						CurrentActivationInfo,
						HitEffectHandle,
						TargetDataHandle
					);
			}*/
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - MeleeWeaponActor is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
}

void UGA_Attack::AddHitCues(const FGameplayAbilityTargetDataHandle& TargetDataHandle,TSubclassOf<UGA_ReceiveHit> InHitAbilityClass)
{
	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();

	FGameplayCueParameters VisualCueParams;

	if(!CurrentAttackData->HitEnemyVisualCueClassArray.IsEmpty())
	{
		for(TSubclassOf VisualCueClass: CurrentAttackData->HitEnemyVisualCueClassArray)
		{
			if(VisualCueClass)
			{
				UFTAVisualCueObject* VisualCueCDO = VisualCueClass->GetDefaultObject<UFTAVisualCueObject>();
				if(VisualCueCDO)
				{
					VisualCueParams.SourceObject = VisualCueCDO;
					VisualCueParams.EffectCauser = TargetActor;
					VisualCueParams.Location = TargetDataHandle.Get(0)->GetHitResult()->Location;
				
					if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(VisualCueCDO->VisualCueTag))
					{
						K2_AddGameplayCueWithParams(VisualCueCDO->VisualCueTag, VisualCueParams);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::AddMeleeHitCues - VisualCueTag is invalid"));
					}
				}
			}
		}
	}
	
	FGameplayCueParameters SoundCueParams;
	if(!CurrentAttackData->HitEnemySoundCueClassArray.IsEmpty())
	{
		for(TSubclassOf SoundCueClass: CurrentAttackData->HitEnemySoundCueClassArray)
		{
			if(SoundCueClass)
			{
				UFTASoundCueObject* SoundCueCDO = SoundCueClass->GetDefaultObject<UFTASoundCueObject>();
				if(SoundCueCDO)
				{
					SoundCueParams.SourceObject = SoundCueCDO;
					SoundCueParams.EffectCauser = TargetActor;
				
					if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(SoundCueCDO->SoundCueTag))
					{
						K2_AddGameplayCueWithParams(SoundCueCDO->SoundCueTag, SoundCueParams);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::SoundCueTag - HitCueTag is invalid"));
					}
				}
			}
		}
	}
}

void UGA_Attack::SendHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FHitDataInfo& HitData)
{

	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	OnHitEventData.Instigator = GetAvatarActorFromActorInfo();
	OnHitEventData.Target = TargetActor;
	OnHitEventData.ContextHandle.AddHitResult(*TargetDataHandle.Get(0)->GetHitResult());
	
	UHitEventObject* HitInfoObj = NewObject<UHitEventObject>(this);
	HitInfoObj->HitData.Instigator = GetAvatarActorFromActorInfo();
	
	HitInfoObj->HitData.Direction = HitData.Direction;
	

	if(CurrentMoveToLocationAsset)
	{
		if(CurrentAttackData->SupplyMovementDataOnHit)
		{
			HitInfoObj->HitData.MoveToLocationData = CurrentMoveToLocationAsset;
			HitInfoObj->HitData.MoveToLocationData->EndLocationVector = EndLocationVector;

			/*DrawDebugSphere(
				GetWorld(),
				TargetEndLocation,
				25.0f,        
				12,           
				FColor::Yellow,
				false,      
				2.0f          
				);*/
			
		}	
	}

	if(HitData.MoveToLocationData)
	{
		HitInfoObj->HitData.MoveToLocationData = HitData.MoveToLocationData;
	}
	
	
	if(WeaponActors.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - MeleeWeaponActor is Null"));
		return;
	}
	
	if(!OnHitEventData.OptionalObject)
	{
		OnHitEventData.OptionalObject = HitInfoObj;
	}
	
	if (HitData.HitAbilityClass)
	{
		const UGA_ReceiveHit* const CDO = HitData.HitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
		if (CDO)
		{
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CDO->ReceiveHitTag))
			{
				OnHitEventData.EventTag = CDO->ReceiveHitTag;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::SendMeleeHitGameplayEvents - HitReactionTag is invalid"));
			}
		}
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, OnHitEventData.EventTag, OnHitEventData);
	OnHitEventData.OptionalObject = nullptr;
	EndLocationVector = FVector::ZeroVector;
}

UFTAAbilityDataAsset* UGA_Attack::SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets)
{
	return Super::SelectAbilityAsset(InAbilityAssets);
}


void UGA_Attack::ExtractAssetProperties(UFTAAbilityDataAsset* InAbilityAsset)
{
	Super::ExtractAssetProperties(InAbilityAsset);
	
	if(!InAbilityAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_Attack::ExtractAssetProperties - InAbilityAsset is null"));
		return;
	}
	
	UAttackAbilityDataAsset* AttackAsset = Cast<UAttackAbilityDataAsset>(InAbilityAsset);

	if(!AttackAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_Attack::ExtractAssetProperties - AttackAsset is null"));
		return;
	}

	//Damage
	if(AttackAsset->ApplyDamageEffect)
	{
		CurrentAttackData->ApplyDamageEffect = AttackAsset->ApplyDamageEffect;
	}

	// Hit Reactions
	if (!AttackAsset->PossibleHitReactions.IsEmpty())
	{
		if (!CurrentAttackData)
		{
			UE_LOG(LogTemp, Error, TEXT("1 UGA_Attack::ExtractAssetProperties - CurrentAttackData is null"));
			return;
		}

		for (const FHitDataInfo& HitData : AttackAsset->PossibleHitReactions)
		{
			if (HitData.HitAbilityClass)
			{
				CurrentAttackData->PossibleHitReactions.Insert(HitData, 0);
			}
		}
	}

	//Hit visuals
	if(!AttackAsset->HitEnemyVisualCueClassArray.IsEmpty())
	{
		for(TSubclassOf VisualCueClass: AttackAsset->HitEnemyVisualCueClassArray)
		{
			if(VisualCueClass && VisualCueClass->IsValidLowLevel())
			{
				CurrentAttackData->HitEnemyVisualCueClassArray.Add(VisualCueClass);
			}
		}
	}

	//Hit sounds
	if(!AttackAsset->HitEnemySoundCueClassArray.IsEmpty())
	{
		for(TSubclassOf SoundCueClass: AttackAsset->HitEnemySoundCueClassArray)
		{
			if(SoundCueClass && SoundCueClass->IsValidLowLevel())
			{
				CurrentAttackData->HitEnemySoundCueClassArray.Add(SoundCueClass);
			}
		}
	}
}

void UGA_Attack::PerformAbility(UFTAAbilityDataAsset* InAbilityAsset)
{
	Super::PerformAbility(InAbilityAsset);
	
	int32 CurrentComboIndex = ComboManagerComponent->GetCurrentComboIndex();
	
	ComboManagerComponent->GetCurrentComboContainer().AddTag(InAbilityAsset->UniqueIdentifierTag);
	ComboManagerComponent->SetCurrentComboIndex(CurrentComboIndex + 1);
	ComboManagerComponent->PauseCurrentAttack = false;
	
}

void UGA_Attack::SetRuntimeAbilityData(UFTAAbilityDataAsset* InAbilityRuntimeData)
{
	Super::SetRuntimeAbilityData(InAbilityRuntimeData);

	if(!InAbilityRuntimeData)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_Attack::SetRuntimeAbilityData - InAbilityRuntimeData is null"))
		return;
	}
	
	UAttackAbilityDataAsset* AttackAsset = Cast<UAttackAbilityDataAsset>(InAbilityRuntimeData);

	if(!AttackAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_Attack::SetRuntimeAbilityData - AttackAsset is null"))
		return;
	}
	
	if (!AttackAsset->PossibleHitReactions.IsEmpty())
	{
		if (!CurrentAttackData)
		{
			UE_LOG(LogTemp, Error, TEXT("UGA_Attack::SetRuntimeAbilityData - CurrentAttackData is null"));
			return;
		}

		for (const FHitDataInfo& HitData : AttackAsset->PossibleHitReactions)
		{
			if (HitData.HitAbilityClass)
			{
				CurrentAttackData->PossibleHitReactions.Insert(HitData, 0);
			}
		}
	}

	//Sometimes null here
	if(CurrentAttackData)
	{
		CurrentAttackData->SupplyMovementDataOnHit = AttackAsset->SupplyMovementDataOnHit;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("3 UGA_Attack::SetRuntimeAbilityData - CurrentAttackData is null"));
		return;
	}
	
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


	

