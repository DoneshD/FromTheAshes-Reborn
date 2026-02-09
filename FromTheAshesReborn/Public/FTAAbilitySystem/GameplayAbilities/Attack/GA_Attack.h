#pragma once

#include "CoreMinimal.h"
#include "DataAsset/AttackAbilityDataAsset.h"
#include "FTAAbilitySystem/GameplayAbilities/GA_FromEquipment.h"
#include "GA_Attack.generated.h"


struct FMeleeAttackForms;
class AWeaponActorBase;
class UAttackAbilityDataAsset;
class UCentralStateComponent;
class UComboManagerComponent;


UCLASS()
class FROMTHEASHESREBORN_API UGA_Attack : public UGA_FromEquipment
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	TArray<TObjectPtr<AWeaponActorBase>> WeaponActors;

	UPROPERTY()
	TObjectPtr<UAttackAbilityDataAsset> CurrentAttackData;

	UPROPERTY(BlueprintReadWrite, Category = "Attack Data")
	FGameplayEventData OnHitEventData;

protected:

	UGA_Attack(const FObjectInitializer& = FObjectInitializer::Get());

	virtual void OnAbilityTick(float DeltaTime) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION()
	virtual void OnHitAdded(FHitResult LastItem);

	virtual FGameplayAbilityTargetDataHandle AddHitResultToTargetData(const FHitResult& LastItem);
	virtual void ExecuteHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	virtual void GrantHitAbility(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);
	
	virtual void ApplyHitEffects(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);
	virtual void AddHitCues(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Attack Ability")
	virtual void SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FHitDataInfo& HitData);
	
public:
	

	UFUNCTION(BlueprintCallable, Category = "FTAAbility")
	virtual UFTAAbilityDataAsset* SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets) override;
	
	virtual void ExtractAssetProperties(UFTAAbilityDataAsset* InAbilityAsset) override;
	virtual void PerformAbility(UFTAAbilityDataAsset* InAbilityAsset) override;

	virtual void SetRuntimeAbilityData(UFTAAbilityDataAsset* InAbilityRuntimeData) override;
	
	virtual void PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage) override;

	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

};
