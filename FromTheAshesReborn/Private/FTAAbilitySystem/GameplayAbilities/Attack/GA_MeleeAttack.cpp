#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeAttack.h"

#include "CombatComponents/ComboManagerComponent.h"
#include "DataAsset/MeleeAbilityDataAsset.h"
#include "FTAAbilitySystem/GameplayCues/SlashCueObject.h"
#include "FTAAbilitySystem/GameplayCues/WeaponCueObject.h"
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
	CurrentMeleeAttackData = DefaultMeleeAttackData;
	
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
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Env")))
	{
		FGameplayCueParameters HitEnvironemtCueParams;

		if(!CurrentMeleeAttackData->AttackData.HitEnvironmentVisualCueClassArray.IsEmpty())
		{
			for(TSubclassOf HitEnvironmentCueClass : CurrentMeleeAttackData->AttackData.HitEnvironmentVisualCueClassArray)
			{
				if(HitEnvironmentCueClass)
				{
					UWeaponCueObject* EnvCueCDO = HitEnvironmentCueClass->GetDefaultObject<UWeaponCueObject>();
					if(EnvCueCDO)
					{
						HitEnvironemtCueParams.SourceObject = EnvCueCDO;
						if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(EnvCueCDO->VisualCueTag))
						{
							K2_AddGameplayCueWithParams(EnvCueCDO->VisualCueTag, HitEnvironemtCueParams);
						}
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("EnvCueCDO null"));
						return;
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Empyu null"));
		
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

TObjectPtr<USlashCueObject> UGA_MeleeAttack::AddTrailCue()
{
	FGameplayCueParameters SlashCueParams;
	if(CurrentMeleeAttackData->SlashTrailCue)
	{
		USlashCueObject* CueCDO = CurrentMeleeAttackData->SlashTrailCue->GetDefaultObject<USlashCueObject>();
		if(CueCDO)
		{
			SlashCueParams.SourceObject = CueCDO;
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CueCDO->SlashCueInfo.SlashCueTag))
			{
				K2_AddGameplayCueWithParams(CueCDO->SlashCueInfo.SlashCueTag, SlashCueParams);
				return CueCDO;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CueCDO null"));
			return nullptr;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::EventMontageReceived - Slash Cue class is invalid"));
		return nullptr;
	}
	return nullptr;
}

TObjectPtr<UWeaponCueObject> UGA_MeleeAttack::AddMeleeTrailCue()
{
	FGameplayCueParameters MeleeTrailCueParams;

	if(!CurrentMeleeAttackData->AttackData.MeleeTrailCueClassArray.IsEmpty())
	{
		for(TSubclassOf TrailCueClass : CurrentMeleeAttackData->AttackData.MeleeTrailCueClassArray)
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
		UE_LOG(LogTemp, Error, TEXT("Empty null"));
		
	}
	return nullptr;
}


void UGA_MeleeAttack::PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage)
{
	Super::PlayAbilityAnimMontage(AnimMontage);
}
