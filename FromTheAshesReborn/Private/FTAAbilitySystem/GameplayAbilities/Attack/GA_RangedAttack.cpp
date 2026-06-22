#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_RangedAttack.h"

#include "CombatComponents/ComboManagerComponent.h"
#include "DataAsset/RangedAbilityDataAsset.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/GameplayCues/FTASoundCueObject.h"
#include "FTACustomBase/FTACharacter.h"
#include "FTAAbilitySystem/GameplayCues/WeaponCueObject.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TargetingSystem/TargetingSystemComponent.h"

UGA_RangedAttack::UGA_RangedAttack(const FObjectInitializer&)
{
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("TestTag.Tag5"));
}

bool UGA_RangedAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_RangedAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{

	if(!DefaultRangedAttackData)
	{
		UE_LOG(LogTemp, Error, TEXT("Default Melee Attack Data must be set"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	CurrentRangedAttackData = DuplicateObject<URangedAbilityDataAsset>(DefaultRangedAttackData,this);
	CurrentAttackData = DuplicateObject<URangedAbilityDataAsset>(DefaultRangedAttackData,this);
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	TargetingSystemComponent = GetFTACharacterFromActorInfo()->FindComponentByClass<UTargetingSystemComponent>();

	if(!TargetingSystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetingComponent is null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}

	
}

void UGA_RangedAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_RangedAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if(CurrentMuzzleVisualCueCDO)
	{
		K2_RemoveGameplayCue(CurrentMuzzleVisualCueCDO->VisualCueTag);
		CurrentMuzzleVisualCueCDO = nullptr;
	}
		
	if(CurrentMuzzleSoundCueCDO)
	{
		K2_RemoveGameplayCue(CurrentMuzzleSoundCueCDO->SoundCueTag);
		CurrentMuzzleSoundCueCDO = nullptr;
	}
	
}

void UGA_RangedAttack::RangedTargetFound(TObjectPtr<AActor> Target)
{
	FHitResult LastItem;
	
	if (Target && Target->Implements<UAbilitySystemInterface>())
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Target);
		UAbilitySystemComponent* TargetASC = AbilitySystemInterface->GetAbilitySystemComponent();

		//TODO: Temporary, change later
		AEnemyBaseCharacter* Enemy = Cast<AEnemyBaseCharacter>(Cast<AEnemyBaseCharacter>(GetFTACharacterFromActorInfo()));

		if(Enemy)
		{
			AEnemyBaseCharacter* TargetEnemy = Cast<AEnemyBaseCharacter>(Target);
			if(!TargetEnemy)
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
				return;
			}
			
		}

		if (TargetASC)
		{
			//Voodoo magic here??
			LastItem.HitObjectHandle = Target;
			
			FGameplayAbilityTargetDataHandle TargetHitDataHandle = AddHitResultToTargetData(LastItem);
			if(TargetHitDataHandle.Num() > 0 && TargetHitDataHandle.Get(0))
			{
				
				// FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetFTACharacterFromActorInfo()->GetActorLocation(),
				// Target->GetActorLocation());
				//
				//
				// FRotator NewRotation = FMath::RInterpTo(
				// 	GetFTACharacterFromActorInfo()->GetActorRotation(),
				// 	LookAtRotation,
				// 	GetWorld()->GetDeltaSeconds(),
				// 	5.0
				// );
				//
				// GetFTACharacterFromActorInfo()->SetActorRotation(LookAtRotation);
				
				ExecuteHitLogic(TargetHitDataHandle);
			}
		}
	}
}

void UGA_RangedAttack::FireShot()
{
	
	TargetingSystemComponent->ClosestTargetDistance = TargetingSystemComponent->MinimumDistanceToEnable;
	
	const TArray<AActor*> Actors = GetAllActorsOfClass(TargetableActors);

	if(Actors.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Actors are null"));
		return;
	}

	CurrentMuzzleVisualCueCDO = AddMuzzleVisualCue();
	CurrentMuzzleSoundCueCDO = AddMeleeTrailSoundCue();
	
	TargetActor = FindNearestTargetToActor(Actors);

	if(TargetActor)
	{
		RangedTargetFound(TargetActor);
	}
}

UWeaponCueObject* UGA_RangedAttack::AddMuzzleVisualCue()
{
	FGameplayCueParameters VisualMuzzleCueParams;

	if(!CurrentRangedAttackData->MuzzleVisualCueArray.IsEmpty())
	{
		for(TSubclassOf MuzzleVisualCueClass : CurrentRangedAttackData->MuzzleVisualCueArray)
		{
			if(MuzzleVisualCueClass)
			{
				UWeaponCueObject* MuzzleVisualCueCDO = MuzzleVisualCueClass->GetDefaultObject<UWeaponCueObject>();
				if(MuzzleVisualCueCDO)
				{
					VisualMuzzleCueParams.SourceObject = MuzzleVisualCueCDO;
					if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(MuzzleVisualCueCDO->VisualCueTag))
					{
						K2_AddGameplayCueWithParams(MuzzleVisualCueCDO->VisualCueTag, VisualMuzzleCueParams);
						return MuzzleVisualCueCDO;
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("MuzzleCueCDO visual null"));
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

UFTASoundCueObject* UGA_RangedAttack::AddMeleeTrailSoundCue()
{
	FGameplayCueParameters SoundMuzzleCueParams;

	if(!CurrentRangedAttackData->MuzzleSoundCueClassArray.IsEmpty())
	{
		for(TSubclassOf MuzzleSoundCueClass : CurrentRangedAttackData->MuzzleSoundCueClassArray)
		{
			if(MuzzleSoundCueClass)
			{
				UFTASoundCueObject* MuzzleSoundCueCDO = MuzzleSoundCueClass->GetDefaultObject<UFTASoundCueObject>();
				if(MuzzleSoundCueCDO)
				{
					SoundMuzzleCueParams.SourceObject = MuzzleSoundCueCDO;
					if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(MuzzleSoundCueCDO->SoundCueTag))
					{
						K2_AddGameplayCueWithParams(MuzzleSoundCueCDO->SoundCueTag, SoundMuzzleCueParams);
						return MuzzleSoundCueCDO;
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("MuzzleCueCDO sound null"));
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

void UGA_RangedAttack::PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage)
{
	Super::PlayAbilityAnimMontage(AnimMontage);
}

void UGA_RangedAttack::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_RangedAttack::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);
}

void UGA_RangedAttack::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("EventTag.FireShot")))
	{
		FireShot();
		
	}
}

FGameplayAbilityTargetDataHandle UGA_RangedAttack::AddHitResultToTargetData(const FHitResult& LastItem)
{
	return Super::AddHitResultToTargetData(LastItem);
}

void UGA_RangedAttack::ExecuteHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{

	if(UseTempCueObjects)
	{
		AActor* TargetActorCue = TargetDataHandle.Get(0)->GetHitResult()->GetActor();

		FGameplayCueParameters VisualCueParams;

		if(TempVisualCueObject)
		{
			VisualCueParams.SourceObject = TempVisualCueObject;
			VisualCueParams.EffectCauser = TargetActorCue;
			VisualCueParams.Location = TargetDataHandle.Get(0)->GetHitResult()->Location;
				
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(TempVisualCueObject->VisualCueTag))
			{
				K2_AddGameplayCueWithParams(TempVisualCueObject->VisualCueTag, VisualCueParams);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::AddMeleeHitCues - VisualCueTag is invalid"));
			}
		}
	}

	Super::ExecuteHitLogic(TargetDataHandle);
	
}

UFTAAbilityDataAsset* UGA_RangedAttack::SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets)
{
	Super::SelectAbilityAsset(InAbilityAssets);

	TArray<UFTAAbilityDataAsset*> RangedAbilityDataAssets;

	for (UFTAAbilityDataAsset* Asset : InAbilityAssets)
	{
		if (URangedAbilityDataAsset* RangedAsset = Cast<URangedAbilityDataAsset>(Asset))
		{
			RangedAbilityDataAssets.Add(RangedAsset);
		}
	}

	if(TObjectPtr<UFTAAbilityDataAsset> AbilityDataAsset = ComboManagerComponent->GetRangedAssetByRequirements(RangedAbilityDataAssets))
	{
		return AbilityDataAsset;
	}
	
	return nullptr;
}

void UGA_RangedAttack::ExtractAssetProperties(UFTAAbilityDataAsset* InAbilityAsset)
{
	Super::ExtractAssetProperties(InAbilityAsset);

	URangedAbilityDataAsset* RangedAttackAsset = Cast<URangedAbilityDataAsset>(InAbilityAsset);

	if(!RangedAttackAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("AttackAsset is null"));
		return;
	}
	
	if(!RangedAttackAsset->MuzzleVisualCueArray.IsEmpty())
	{
		for(TSubclassOf MuzzleVisualCueClass: RangedAttackAsset->MuzzleVisualCueArray)
		{
			if(MuzzleVisualCueClass && MuzzleVisualCueClass->IsValidLowLevel())
			{
				CurrentRangedAttackData->MuzzleVisualCueArray = RangedAttackAsset->MuzzleVisualCueArray;
			}
		}
	}

	if(!RangedAttackAsset->MuzzleSoundCueClassArray.IsEmpty())
	{
		for(TSubclassOf MuzzleSoundCueClass: RangedAttackAsset->MuzzleSoundCueClassArray)
		{
			if(MuzzleSoundCueClass && MuzzleSoundCueClass->IsValidLowLevel())
			{
				CurrentRangedAttackData->MuzzleSoundCueClassArray.Add(MuzzleSoundCueClass);
			}
		}
	}
	
}

void UGA_RangedAttack::SetRuntimeAbilityData(UFTAAbilityDataAsset* InAbilityRuntimeData)
{
	Super::SetRuntimeAbilityData(InAbilityRuntimeData);

	URangedAbilityDataAsset* RangedAttackAsset = Cast<URangedAbilityDataAsset>(InAbilityRuntimeData);

	if(!RangedAttackAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("AttackAsset is null"));
		return;
	}
	
	if(!RangedAttackAsset->MuzzleVisualCueArray.IsEmpty())
	{
		for(TSubclassOf MuzzleVisualCueClass: RangedAttackAsset->MuzzleVisualCueArray)
		{
			if(MuzzleVisualCueClass && MuzzleVisualCueClass->IsValidLowLevel())
			{
				CurrentRangedAttackData->MuzzleVisualCueArray = RangedAttackAsset->MuzzleVisualCueArray;
			}
		}
	}

	if(!RangedAttackAsset->MuzzleSoundCueClassArray.IsEmpty())
	{
		for(TSubclassOf MuzzleSoundCueClass: RangedAttackAsset->MuzzleSoundCueClassArray)
		{
			if(MuzzleSoundCueClass && MuzzleSoundCueClass->IsValidLowLevel())
			{
				CurrentRangedAttackData->MuzzleSoundCueClassArray.Add(MuzzleSoundCueClass);
			}
		}
	}
	
}

void UGA_RangedAttack::PerformAbility(UFTAAbilityDataAsset* InAbilityAsset)
{
	Super::PerformAbility(InAbilityAsset);
}

TArray<AActor*> UGA_RangedAttack::GetAllActorsOfClass(TSubclassOf<AActor> ActorClass) const
{
	return TargetingSystemComponent->GetAllActorsOfClass(ActorClass);
}

AActor* UGA_RangedAttack::FindNearestTargetToActor(TArray<AActor*> Actors) const
{
	return TargetingSystemComponent->FindNearestTargetToActor(Actors);
}

void UGA_RangedAttack::OnHitAdded(FHitResult LastItem)
{
	Super::OnHitAdded(LastItem);
}

