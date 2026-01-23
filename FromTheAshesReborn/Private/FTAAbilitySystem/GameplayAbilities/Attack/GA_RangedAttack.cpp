#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_RangedAttack.h"

#include "CombatComponents/ComboManagerComponent.h"
#include "DataAsset/RangedAbilityDataAsset.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "FTAAbilitySystem/GameplayCues/RangedOriginCueObject.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TargetingSystem/TargetingSystemComponent.h"

UGA_RangedAttack::UGA_RangedAttack(const FObjectInitializer&)
{
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
	CurrentRangedAttackData = DefaultRangedAttackData;
	
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
			if(TargetEnemy)
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
				return;
			}
		}

		if (TargetASC)
		{
			//Voodoo magic here
			LastItem.HitObjectHandle = Target;
			
			FGameplayAbilityTargetDataHandle TargetHitDataHandle = AddHitResultToTargetData(LastItem);
			if(TargetHitDataHandle.Num() > 0 && TargetHitDataHandle.Get(0))
			{
				ExecuteHitLogic(TargetHitDataHandle);
			}
		}
	}
}

void UGA_RangedAttack::FireShot()
{
	AddRangedOriginCues();
	
	TargetingSystemComponent->ClosestTargetDistance = TargetingSystemComponent->MinimumDistanceToEnable;
	
	const TArray<AActor*> Actors = GetAllActorsOfClass(TargetableActors);

	if(Actors.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Actors are null"));
		return;
	}
	
	TargetActor = FindNearestTargetToActor(Actors);

	if(TargetActor)
	{
		RangedTargetFound(TargetActor);
	}
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
	
	//Origin Cue
	if(RangedAttackAsset->OriginCueClass && RangedAttackAsset->OriginCueClass->IsValidLowLevel())
	{
		CurrentRangedAttackData->OriginCueClass = RangedAttackAsset->OriginCueClass;
	}
	
	//Left or right weapon
	CurrentRangedAttackData->Hand = RangedAttackAsset->Hand;
	
	
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

void UGA_RangedAttack::AddRangedOriginCues()
{
	FGameplayCueParameters OriginCueParams;
	if(CurrentRangedAttackData->OriginCueClass)
	{
		URangedOriginCueObject* CueCDO = CurrentRangedAttackData->OriginCueClass->GetDefaultObject<URangedOriginCueObject>();
		CueCDO->RangedOriginCueInfo.Hand = CurrentRangedAttackData->Hand;

		if(CueCDO)
		{
			OriginCueParams.SourceObject = CueCDO;
			OriginCueParams.EffectCauser = TargetActor;
			
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CueCDO->RangedOriginCueInfo.OriginCueTag))
			{
				K2_AddGameplayCueWithParams(CueCDO->RangedOriginCueInfo.OriginCueTag, OriginCueParams);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::AddMeleeHitCues - HitCueTag is invalid"));
			}
		}
	}
}
