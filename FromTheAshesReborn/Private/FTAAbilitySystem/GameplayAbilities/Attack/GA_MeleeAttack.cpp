#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeAttack.h"

#include "CombatComponents/ComboManagerComponent.h"
#include "DataAsset/MeleeAbilityDataAsset.h"
#include "FTAAbilitySystem/GameplayCues/FTASoundCueObject.h"
#include "FTAAbilitySystem/GameplayCues/WeaponCueObject.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
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
	if(!DefaultMeleeAttackData)
	{
		UE_LOG(LogTemp, Error, TEXT("Default Melee Attack Data must be set"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}

	// CurrentAttackData = DefaultMeleeAttackData;
	// CurrentMeleeAttackData = DefaultMeleeAttackData;

	CurrentAttackData = DuplicateObject<UMeleeAbilityDataAsset>(DefaultMeleeAttackData,this);
	CurrentMeleeAttackData = DuplicateObject<UMeleeAbilityDataAsset>(DefaultMeleeAttackData,this);

	
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
	CurrentMeleeAttackData = nullptr;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	// CurrentAttackData->AttackDirectionStruct.AttackDirection = ESpatialDirection::None;
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
	
	UWeaponCueObject* CueCDO = nullptr;
	
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.BeginSlash")))
	{
		StartMeleeWeaponTrace();
		CueCDO = AddMeleeTrailCue();

	}
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.EndSlash")))
	{
		EndMeleeWeaponTrace();
		if(CueCDO)
		{
			K2_RemoveGameplayCue(CueCDO->VisualCueTag);
		}
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
	
	if(!InAbilityAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeAttack::ExtractAssetProperties - InAbilityAsset is null"));
		return;
	}
	
	UMeleeAbilityDataAsset* MeleeAsset = Cast<UMeleeAbilityDataAsset>(InAbilityAsset);
	
	if(!MeleeAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeAttack::ExtractAssetProperties - MeleeAbilityDataAsset is null"));
		return;
	}

	//Trail visual
	if(!MeleeAsset->BaseTrailVisualCueClassArray.IsEmpty())
	{
		for(TSubclassOf TrailVisualCueClass: MeleeAsset->BaseTrailVisualCueClassArray)
		{
			if(TrailVisualCueClass && TrailVisualCueClass->IsValidLowLevel())
			{
				CurrentMeleeAttackData->BaseTrailVisualCueClassArray = MeleeAsset->BaseTrailVisualCueClassArray;
			}
		}
	}

	if(!MeleeAsset->TrailSoundCueClassArray.IsEmpty())
	{
		for(TSubclassOf TrailSoundCueClass: MeleeAsset->TrailSoundCueClassArray)
		{
			if(TrailSoundCueClass && TrailSoundCueClass->IsValidLowLevel())
			{
				CurrentMeleeAttackData->TrailSoundCueClassArray.Add(TrailSoundCueClass);
			}
		}
	}
}

void UGA_MeleeAttack::SetRuntimeAbilityData(UFTAAbilityDataAsset* InAbilityRuntimeData)
{
	Super::SetRuntimeAbilityData(InAbilityRuntimeData);

	UMeleeAbilityDataAsset* MeleeAsset = Cast<UMeleeAbilityDataAsset>(InAbilityRuntimeData);

	if(!MeleeAsset)
	{
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

TObjectPtr<UWeaponCueObject> UGA_MeleeAttack::AddMeleeTrailCue()
{
	FGameplayCueParameters MeleeTrailCueParams;

	if(!CurrentMeleeAttackData->BaseTrailVisualCueClassArray.IsEmpty())
	{
		for(TSubclassOf TrailCueClass : CurrentMeleeAttackData->BaseTrailVisualCueClassArray)
		{
			if(TrailCueClass)
			{
				UWeaponCueObject* TrailCueCDO = TrailCueClass->GetDefaultObject<UWeaponCueObject>();
				if(TrailCueCDO)
				{
					MeleeTrailCueParams.SourceObject = TrailCueCDO;
					if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(TrailCueCDO->VisualCueTag))
					{
						K2_AddGameplayCueWithParams(TrailCueCDO->VisualCueTag, MeleeTrailCueParams);
						return TrailCueCDO;
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("TrailCueCDO null"));
					return nullptr;
				}
			}
		}
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("Empty null"));
	}
	return nullptr;
}


void UGA_MeleeAttack::PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage)
{
	Super::PlayAbilityAnimMontage(AnimMontage);
}
