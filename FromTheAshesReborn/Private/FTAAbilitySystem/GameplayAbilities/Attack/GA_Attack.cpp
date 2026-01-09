#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_Attack.h"

#include "Camera/CameraSystemComponent.h"
#include "CombatComponents/ComboManagerComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
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

	ComboManagerComponent = GetFTACharacterFromActorInfo()->ComboManagerComponent;

	CentralStateComponent = GetFTACharacterFromActorInfo()->CentralStateComponent;

	if(!CentralStateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - CentralStateComponent is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	if(!ComboManagerComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - ComboManagerComponent is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}


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

	for (AWeaponActorBase* WeaponActor : WeaponActors)
	{
		if(WeaponActor)
		{
			if(WeaponActor->TracingComponent)
			{
				WeaponActor->TracingComponent->OnItemAdded.AddDynamic(this, &UGA_Attack::OnHitAdded);

				//Refactor for ranged
				// WeaponActor->TracingComponent->BoxHalfSize = FVector(
				// 	DefaultAttackData.WeaponTraceSizeStruct.WeaponTraceSize.X,
				// 	DefaultAttackData.WeaponTraceSizeStruct.WeaponTraceSize.Y,
				// 	DefaultAttackData.WeaponTraceSizeStruct.WeaponTraceSize.Z);
				
			}
		}
	}
	
	if(!AttackComboTypes.NormalAttacks.IsValidIndex(0) || AttackComboTypes.NormalAttacks.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Melee Attack Assets is invalid"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	PerformAttack(AttackComboTypes);
	
	UCameraSystemComponent* CSC = GetFTACharacterFromActorInfo()->FindComponentByClass<UCameraSystemComponent>();
	if(!CSC)
	{
		return;
	}
	
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
	
}

void UGA_Attack::PerformAttack(FAttackComboType& AttackTypes)
{
	if(NonMontageAbility)
	{
		return;
	}
	
	TObjectPtr<UAttackAbilityDataAsset> MatchingDataAsset;
	bool DataAssetFound = ComboManagerComponent->FindMatchingMeleeAssetToTagContainer(AttackTypes, MatchingDataAsset);
	
	if(!DataAssetFound)
	{
		UE_LOG(LogTemp, Error, TEXT("DA NOT found"));
		ResetAttack();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	if(!MatchingDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("MatchingDataAsset is NULL"));
		ResetAttack();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	if (MatchingDataAsset->MontageToPlay)
	{
		int32 CurrentComboIndex = ComboManagerComponent->GetCurrentComboIndex();
		
		ComboManagerComponent->GetCurrentComboContainer().AddTag(MatchingDataAsset->UniqueIdentifierTag);

		// GetFTAAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag()
		ComboManagerComponent->SetCurrentComboIndex(CurrentComboIndex + 1);
		ComboManagerComponent->PauseCurrentAttack = false;

		CurrentAttackData = DefaultAttackData;
		
		// ExtractMeleeAssetProperties(MatchingDataAsset);
		PlayAbilityAnimMontage(MatchingDataAsset->MontageToPlay);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AttackMontageToPlay is NULL"));
		ResetAttack();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}           
}

void UGA_Attack::ResetAttack()
{
	
}


	

