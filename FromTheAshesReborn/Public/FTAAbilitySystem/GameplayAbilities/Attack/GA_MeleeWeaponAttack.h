#pragma once

#include "CoreMinimal.h"
#include "../GA_FromEquipment.h"
#include "CombatComponents/CombatStateComponent.h"
#include "CombatComponents/MeleePropertiesComponent.h"
#include "EventObjects/HitEventObject.h"
#include "GA_MeleeWeaponAttack.generated.h"

class UGA_ReceiveHit;
struct FMeleeAttackDataStruct;
class UMeleePropertiesComponent;
class UNiagaraSystem;
class UComboManagerComponent;
class UMeleeAbilityDataAsset;
class UFTAAbilityDataAsset;
class UMeleeAttackDataAsset;
class AWeaponActorBase;
class UMeleeWeaponInstance;

USTRUCT(BlueprintType)
struct FMeleeAttackForms
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee Attack Forms")
	TArray<TObjectPtr<UMeleeAbilityDataAsset>> NormalAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee Attack Forms")
	TArray<TObjectPtr<UMeleeAbilityDataAsset>> PauseAttacks;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee Attack Forms")
	TArray<TObjectPtr<UMeleeAbilityDataAsset>> VariantAttacks;
	
};

UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeWeaponAttack : public UGA_FromEquipment
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	TObjectPtr<AFTACharacter> FTAChar;

	UPROPERTY()
	TObjectPtr<UComboManagerComponent> ComboManagerComponent;

	UPROPERTY()
	TObjectPtr<UMeleePropertiesComponent> MeleePropertiesComponent;

	UPROPERTY()
	TObjectPtr<AWeaponActorBase> MeleeWeaponActor;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attacks")
	FMeleeAttackForms MeleeAttackAssets;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UGameplayEffect> ApplyDamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
	TArray<TSubclassOf<UGA_ReceiveHit>> PossibleHitReactions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	FMeleeAttackDataStruct FinalAttackData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
	FHitReactionDataStruct FinalHitData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "After Image")
	float AfterImageDistance = 200.0f;
	
	FGameplayEventData OnHitEventData;


protected:

	UGA_MeleeWeaponAttack(const FObjectInitializer& = FObjectInitializer::Get());

	virtual void OnAbilityTick(float DeltaTime) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void SpawnAfterImage();
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "FTAAbility")
	void ResetMeleeAttack();

	UFUNCTION(BlueprintCallable, Category = "FTAAbility")
	void PerformMeleeAttack(FMeleeAttackForms& MeleeAttackDataAssets);
	void StartMeleeWeaponTrace();
	void EndMeleeWeaponTrace();

	UFUNCTION(BlueprintCallable, Category = "FTAAbility")
	UMeleeWeaponInstance* GetMeleeWeaponInstance() const;

	UFUNCTION()
	void SetRuntimeMeleeData(FMeleeAttackDataStruct InMeleeRuntimeData);

protected:
	
	UFUNCTION()
	void OnHitAdded(FHitResult LastItem);

	FGameplayAbilityTargetDataHandle AddHitResultToTargetData(const FHitResult& LastItem);

	void RemoveHitReaction(FGameplayTag RemovalTag, TArray<TSubclassOf<UGA_ReceiveHit>>& TempPossibleHitReactions);

	void SelectHitReaction(UAbilitySystemComponent* TargetASC, UCombatStateComponent* CombatStateComponent, TSubclassOf<UGA_ReceiveHit>&
	                       InHitReactionStruct);
	bool GetTargetStateComponentsAndHitReaction(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit>& InHitReactionStruct);
	
	virtual void ExtractMeleeAssetProperties(TObjectPtr<UMeleeAbilityDataAsset> MeleeAsset);
	virtual void ExecuteMeleeHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	virtual void SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> CurrentHitReactionStruct);
	virtual void ApplyMeleeHitEffects(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> CurrentHitReactionStruct);
	virtual void AddMeleeHitCues(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> CurrentHitReactionStruct);

	virtual void PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage) override;

	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

};
