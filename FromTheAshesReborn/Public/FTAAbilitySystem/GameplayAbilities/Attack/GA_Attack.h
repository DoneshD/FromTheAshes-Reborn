#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayAbilities/GA_FromEquipment.h"
#include "GA_Attack.generated.h"


class AWeaponActorBase;
class UCentralStateComponent;
class UComboManagerComponent;

//Atack Forms

/*USTRUCT(BlueprintType)
struct FMeleeAttackForms
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)

	TArray<TObjectPtr<UMeleeAbilityDataAsset>> NormalAttacks;
	
};*/

UCLASS()
class FROMTHEASHESREBORN_API UGA_Attack : public UGA_FromEquipment
{
	GENERATED_BODY()

protected:

	//Components and Actors
	
	UPROPERTY()
	TObjectPtr<UComboManagerComponent> ComboManagerComponent;

	UPROPERTY()
	TObjectPtr<UCentralStateComponent> CentralStateComponent;

	UPROPERTY()
	TArray<TObjectPtr<AWeaponActorBase>> WeaponActors;

	//Attack Assets and Data

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attacks")
	FMeleeAttackForms MeleeAttackAssets;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	FMeleeAttackDataStruct DefaultAttackData;
	
	FMeleeAttackDataStruct CurrentAttackData;*/

	//Hit Event

	/*UPROPERTY(BlueprintReadWrite, Category = "Attack Data")
	FGameplayEventData OnHitEventData;*/

protected:

	UGA_Attack(const FObjectInitializer& = FObjectInitializer::Get());

	virtual void OnAbilityTick(float DeltaTime) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	/*UFUNCTION()
	void OnHitAdded(FHitResult LastItem);

	FGameplayAbilityTargetDataHandle AddHitResultToTargetData(const FHitResult& LastItem);
	
	virtual void ExtractMeleeAssetProperties(TObjectPtr<UMeleeAbilityDataAsset> MeleeAsset);
	virtual void ExecuteMeleeHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	virtual void ApplyMeleeHitEffects(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Attack Ability")
	virtual void SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);
	
	virtual void GrantHitAbility(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);
	virtual void AddMeleeHitCues(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass);

	virtual void PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage) override;

	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

	void SpawnAfterImage();*/

	
	
public:
	
	/*UFUNCTION(BlueprintCallable, Category = "FTAAbility")
	void ResetMeleeAttack();

	UFUNCTION(BlueprintCallable, Category = "FTAAbility")
	void PerformMeleeAttack(FMeleeAttackForms& MeleeAttackDataAssets);

	UFUNCTION()
	void SetRuntimeMeleeData(FMeleeAttackDataStruct InMeleeRuntimeData);*/
};
