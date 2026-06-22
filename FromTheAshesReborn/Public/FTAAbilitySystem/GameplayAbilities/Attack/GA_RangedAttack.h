#pragma once

#include "CoreMinimal.h"
#include "GA_Attack.h"
#include "GA_RangedAttack.generated.h"

class URangedAbilityDataAsset;
class UTargetingSystemComponent;

UCLASS()
class FROMTHEASHESREBORN_API UGA_RangedAttack : public UGA_Attack
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Temp Cue")
	bool UseTempCueObjects = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Temp Cue", meta=(EditCondition = "UseTempCueObjects"))
	TObjectPtr<UFTAVisualCueObject> TempVisualCueObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Temp Cue")
	TObjectPtr<UFTASoundCueObject> TempRangedSoundCueObject;

protected:

	UPROPERTY()
	TObjectPtr<UTargetingSystemComponent> TargetingSystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	TSubclassOf<AActor> TargetableActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	TObjectPtr<URangedAbilityDataAsset> DefaultRangedAttackData;
	
	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY()
	TObjectPtr<URangedAbilityDataAsset> CurrentRangedAttackData;

	UPROPERTY()
	UWeaponCueObject* CurrentMuzzleVisualCueCDO = nullptr;

	UPROPERTY()
	UFTASoundCueObject* CurrentMuzzleSoundCueCDO = nullptr;

	

protected:

	UGA_RangedAttack(const FObjectInitializer& = FObjectInitializer::Get());
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void RangedTargetFound(TObjectPtr<AActor> Target);

	void FireShot();
	UWeaponCueObject* AddMuzzleVisualCue();
	UFTASoundCueObject* AddMeleeTrailSoundCue();

	virtual void PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage) override;
	
	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

	virtual FGameplayAbilityTargetDataHandle AddHitResultToTargetData(const FHitResult& LastItem) override;
	virtual void ExecuteHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;

	virtual UFTAAbilityDataAsset* SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets) override;
	virtual void ExtractAssetProperties(UFTAAbilityDataAsset* InAbilityAsset) override;
	virtual void SetRuntimeAbilityData(UFTAAbilityDataAsset* InAbilityRuntimeData) override;
	
	virtual void PerformAbility(UFTAAbilityDataAsset* InAbilityAsset) override;

	TArray<AActor*> GetAllActorsOfClass(TSubclassOf<AActor> ActorClass) const;
	AActor* FindNearestTargetToActor(TArray<AActor*> Actors) const;

	virtual void OnHitAdded(FHitResult LastItem) override;

};
