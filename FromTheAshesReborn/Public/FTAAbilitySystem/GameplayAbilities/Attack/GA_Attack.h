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

	//Components and Actors

	UPROPERTY()
	TArray<TObjectPtr<AWeaponActorBase>> WeaponActors;

	//Attack Assets and Data
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	FAttackDataStruct DefaultAttackData;

	UPROPERTY()
	FAttackDataStruct CurrentAttackData;

	//Hit Event

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

	FGameplayAbilityTargetDataHandle AddHitResultToTargetData(const FHitResult& LastItem);
	virtual void ExecuteHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	virtual void GrantHitAbility(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);
	
public:
	

	UFUNCTION(BlueprintCallable, Category = "FTAAbility")
	virtual UFTAAbilityDataAsset* SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets) override;
	virtual void ExtractAssetProperties(UFTAAbilityDataAsset* InAbilityAsset) override;
	virtual void PerformAbility(UFTAAbilityDataAsset* AttackTypes) override;
	
	UFUNCTION(BlueprintCallable, Category = "FTAAbility")
	void ResetAttack();
	
	
//
// 	FGameplayAbilityTargetDataHandle AddHitResultToTargetData(const FHitResult& LastItem);
// 	
// 	virtual void ExtractMeleeAssetProperties(TObjectPtr<UMeleeAbilityDataAsset> MeleeAsset);
// 	virtual void ExecuteMeleeHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
// 	virtual void ApplyMeleeHitEffects(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);
//
// 	UFUNCTION(BlueprintCallable, Category = "Attack Ability")
// 	virtual void SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);
// 	
// 	virtual void GrantHitAbility(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);
// 	virtual void AddMeleeHitCues(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);
//
	virtual void PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage) override;

	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;
// 	
// public:
// 	


	// void SetAbilityRuntimeData(TObjectPtr<UFTAAbilityDataAsset> InAbilityRuntimeData);
};
