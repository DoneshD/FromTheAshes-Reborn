#pragma once

#include "CoreMinimal.h"
#include "GA_Attack.h"
#include "GA_MeleeAttack.generated.h"

class USlashCueObject;
class UMeleeAbilityDataAsset;

USTRUCT(BlueprintType)
struct FMeleeComboType
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UMeleeAbilityDataAsset>> NormalAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UMeleeAbilityDataAsset>> PauseAttacks;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UMeleeAbilityDataAsset>> VariantAttacks;
	
};

UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeAttack : public UGA_Attack
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	TObjectPtr<UMeleeAbilityDataAsset> DefaultMeleeAttackData;
	
	UPROPERTY()
	TObjectPtr<UMeleeAbilityDataAsset> CurrentMeleeAttackData;
	
protected:
	
	UGA_MeleeAttack(const FObjectInitializer& = FObjectInitializer::Get());
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage) override;
	
	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

	virtual UFTAAbilityDataAsset* SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets) override;
	virtual void ExtractAssetProperties(UFTAAbilityDataAsset* InAbilityAsset) override;
	virtual void SetRuntimeAbilityData(UFTAAbilityDataAsset* InAbilityRuntimeData) override;
	
	virtual void PerformAbility(UFTAAbilityDataAsset* InAbilityAsset) override;

	virtual void OnHitAdded(FHitResult LastItem) override;

	void StartMeleeWeaponTrace();
	void EndMeleeWeaponTrace();
	
	TObjectPtr<USlashCueObject> AddTrailCue();

	TObjectPtr<UWeaponCueObject> AddMeleeTrailCue();
	
};
