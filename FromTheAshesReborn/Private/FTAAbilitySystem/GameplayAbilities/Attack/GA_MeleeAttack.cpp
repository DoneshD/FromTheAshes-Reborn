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
	
	CurrentMeleeAttackData = DuplicateObject<UMeleeAbilityDataAsset>(DefaultMeleeAttackData,this);
	CurrentAttackData = DuplicateObject<UMeleeAbilityDataAsset>(DefaultMeleeAttackData,this);
	
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

	for (AWeaponActorBase* WeaponActor : WeaponActors)
	{
		if(WeaponActor)
		{
			if(WeaponActor->TracingComponent)
			{
				WeaponActor->TracingComponent->ToggleTraceCheck(false);
				WeaponActor->TracingComponent->ClearHitArray();
				WeaponActor->TracingComponent->OnItemAdded.RemoveDynamic(this, &UGA_MeleeAttack::OnHitAdded);
				WeaponActor->TracingComponent->OnItemAdded.RemoveAll(this);
			}
		}
	}

	if(GetFTACharacterFromActorInfo()->LimbTracingComponent)
	{
		GetFTACharacterFromActorInfo()->LimbTracingComponent->ToggleTraceCheck(false);
		GetFTACharacterFromActorInfo()->LimbTracingComponent->ClearHitArray();
		GetFTACharacterFromActorInfo()->LimbTracingComponent->OnItemAdded.RemoveDynamic(this, &UGA_MeleeAttack::OnHitAdded);
		GetFTACharacterFromActorInfo()->LimbTracingComponent->OnItemAdded.RemoveAll(this);
	}
	
	CurrentMeleeAttackData = nullptr;
	CurrentVisualCueCDO = nullptr;
	CurrentSoundCueCDO = nullptr;
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
	
	
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.BeginSlash")))
	{
		StartMeleeTrace();
		CurrentVisualCueCDO = AddMeleeTrailVisualCue();
		CurrentSoundCueCDO = AddMeleeTrailSoundCue();

	}
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.EndSlash")))
	{
		if(CurrentVisualCueCDO)
		{
			K2_RemoveGameplayCue(CurrentVisualCueCDO->VisualCueTag);
			CurrentVisualCueCDO = nullptr;
		}
		
		if(CurrentSoundCueCDO)
		{
			K2_RemoveGameplayCue(CurrentSoundCueCDO->SoundCueTag);
			CurrentSoundCueCDO = nullptr;
		}
	
		EndMeleeTrace();
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
	if(!MeleeAsset->TrailVisualCueClassArray.IsEmpty())
	{
		for(TSubclassOf TrailVisualCueClass: MeleeAsset->TrailVisualCueClassArray)
		{
			if(TrailVisualCueClass && TrailVisualCueClass->IsValidLowLevel())
			{
				CurrentMeleeAttackData->TrailVisualCueClassArray = MeleeAsset->TrailVisualCueClassArray;
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
	if(MeleeAsset->MeleeSource.MeleeType == EMeleeType::Limb)
	{
		if(CurrentMeleeAttackData)
		{
			CurrentMeleeAttackData->MeleeSource.MeleeType = MeleeAsset->MeleeSource.MeleeType;
			CurrentMeleeAttackData->MeleeSource.Limb = MeleeAsset->MeleeSource.Limb;
		}
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

void UGA_MeleeAttack::StartMeleeTrace()
{
	if(CurrentMeleeAttackData->MeleeSource.MeleeType == EMeleeType::Weapon)
	{
		for (AWeaponActorBase* WeaponActor : WeaponActors)
		{
			//Another instance of crashing here
			if(WeaponActor)
			{
				if(WeaponActor->TracingComponent)
				{
					WeaponActor->TracingComponent->OnItemAdded.AddDynamic(this, &UGA_MeleeAttack::OnHitAdded);

					WeaponActor->TracingComponent->InclusionStringFilter = TEXT("DIH");
					
					WeaponActor->TracingComponent->SetupVariables(GetFTACharacterFromActorInfo()->GetMesh(), nullptr);
					WeaponActor->TracingComponent->MyActorsToIgnore.AddUnique(GetFTACharacterFromActorInfo());
					WeaponActor->TracingComponent->ShouldIgnoreSelf = true;
				
					WeaponActor->TracingComponent->BoxHalfSize = FVector(
					75.0f,
					75.0f,
					75.0f);
					
					WeaponActor->TracingComponent->ToggleTraceCheck(true);

					//TODO: Temp fix - Multiple instances of same weapon actor
					break;
					
				}
			}
		}
	}
	else if(CurrentMeleeAttackData->MeleeSource.MeleeType == EMeleeType::Limb)
	{
		if(GetFTACharacterFromActorInfo()->LimbTracingComponent)
		{
			GetFTACharacterFromActorInfo()->LimbTracingComponent->OnItemAdded.AddDynamic(this, &UGA_MeleeAttack::OnHitAdded);
			
			if(CurrentMeleeAttackData->MeleeSource.Limb == ELimb::LeftLeg)
			{
				GetFTACharacterFromActorInfo()->LimbTracingComponent->InclusionStringFilter = TEXT("LeftLeg");
			}
			else if(CurrentMeleeAttackData->MeleeSource.Limb == ELimb::LeftArm)
			{
				GetFTACharacterFromActorInfo()->LimbTracingComponent->InclusionStringFilter = TEXT("LeftArm");
			}
			else if(CurrentMeleeAttackData->MeleeSource.Limb == ELimb::RightArm)
			{
				GetFTACharacterFromActorInfo()->LimbTracingComponent->InclusionStringFilter = TEXT("RightArm");
			}
			else if(CurrentMeleeAttackData->MeleeSource.Limb == ELimb::RightLeg)
			{
				GetFTACharacterFromActorInfo()->LimbTracingComponent->InclusionStringFilter = TEXT("RightLeg");
			}
		}

		GetFTACharacterFromActorInfo()->LimbTracingComponent->SetupVariables(GetFTACharacterFromActorInfo()->GetMesh(), nullptr);
		GetFTACharacterFromActorInfo()->LimbTracingComponent->MyActorsToIgnore.AddUnique(GetFTACharacterFromActorInfo());
		GetFTACharacterFromActorInfo()->LimbTracingComponent->ShouldIgnoreSelf = true;
			
		GetFTACharacterFromActorInfo()->LimbTracingComponent->BoxHalfSize = FVector(
		75.0f,
		75.0f,
		75.0f);
				
		GetFTACharacterFromActorInfo()->LimbTracingComponent->ToggleTraceCheck(true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("None selected"));
	}
}

void UGA_MeleeAttack::EndMeleeTrace()
{
	for (AWeaponActorBase* WeaponActor : WeaponActors)
	{
		if(WeaponActor)
		{
			if(WeaponActor->TracingComponent)
			{
				WeaponActor->TracingComponent->ToggleTraceCheck(false);
				WeaponActor->TracingComponent->ClearHitArray();
				WeaponActor->TracingComponent->OnItemAdded.RemoveDynamic(this, &UGA_MeleeAttack::OnHitAdded);
				WeaponActor->TracingComponent->OnItemAdded.RemoveAll(this);
			}
		}
	}

	if(GetFTACharacterFromActorInfo()->LimbTracingComponent)
	{
		GetFTACharacterFromActorInfo()->LimbTracingComponent->ToggleTraceCheck(false);
		GetFTACharacterFromActorInfo()->LimbTracingComponent->ClearHitArray();
		GetFTACharacterFromActorInfo()->LimbTracingComponent->OnItemAdded.RemoveDynamic(this, &UGA_MeleeAttack::OnHitAdded);
		GetFTACharacterFromActorInfo()->LimbTracingComponent->OnItemAdded.RemoveAll(this);
	}
	CurrentVisualCueCDO = nullptr;
	CurrentSoundCueCDO = nullptr;
}

TObjectPtr<UWeaponCueObject> UGA_MeleeAttack::AddMeleeTrailVisualCue()
{
	FGameplayCueParameters MeleeTrailCueParams;

	if(!CurrentMeleeAttackData->TrailVisualCueClassArray.IsEmpty())
	{
		for(TSubclassOf TrailCueClass : CurrentMeleeAttackData->TrailVisualCueClassArray)
		{
			if(TrailCueClass)
			{
				UWeaponCueObject* TrailVisualCueCDO = TrailCueClass->GetDefaultObject<UWeaponCueObject>();
				if(TrailVisualCueCDO)
				{
					MeleeTrailCueParams.SourceObject = TrailVisualCueCDO;
					if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(TrailVisualCueCDO->VisualCueTag))
					{
						K2_AddGameplayCueWithParams(TrailVisualCueCDO->VisualCueTag, MeleeTrailCueParams);
						return TrailVisualCueCDO;
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

TObjectPtr<UFTASoundCueObject> UGA_MeleeAttack::AddMeleeTrailSoundCue()
{
	FGameplayCueParameters TrailSoundCueParams;

	if(!CurrentMeleeAttackData->TrailSoundCueClassArray.IsEmpty())
	{
		for(TSubclassOf TrailCueClass : CurrentMeleeAttackData->TrailSoundCueClassArray)
		{
			if(TrailCueClass)
			{
				UFTASoundCueObject* TrailSoundCueCDO = TrailCueClass->GetDefaultObject<UFTASoundCueObject>();
				if(TrailSoundCueCDO)
				{
					TrailSoundCueParams.SourceObject = TrailSoundCueCDO;
					if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(TrailSoundCueCDO->SoundCueTag))
					{
						K2_AddGameplayCueWithParams(TrailSoundCueCDO->SoundCueTag, TrailSoundCueParams);
						return TrailSoundCueCDO;
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("TrailSoundCueCDO null"));
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
