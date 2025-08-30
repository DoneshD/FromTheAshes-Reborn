#pragma once

#include "CoreMinimal.h"
#include "../GA_FromEquipment.h"
#include "CombatComponents/CombatStateComponent.h"
#include "EventObjects/HitEventObject.h"
#include "GA_MeleeWeaponAttack.generated.h"

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

USTRUCT(BlueprintType)
struct FMeleeRuntimeDataStruct
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Runtime Data")
	float TraceSize;

	UPROPERTY(BlueprintReadWrite, Category = "Runtime Data")
	ESpatialDirection HitDirection;

	UPROPERTY(BlueprintReadWrite, Category = "Runtime Data")
	TObjectPtr<UNiagaraSystem> SlashFX;

	UPROPERTY(BlueprintReadWrite, Category = "Runtime Data")
	TObjectPtr<UNiagaraSystem> HitFX;

	UPROPERTY(BlueprintReadWrite, Category = "Runtime Data")
	TSubclassOf<UGameplayEffect> HitReactionEffect;
	
	UPROPERTY(BlueprintReadWrite, Category = "Runtime Data")
	FGameplayTag HitReactionTag;

	UPROPERTY(BlueprintReadWrite, Category = "Runtime Data")
	FHitReactionDataStruct HitReactionStruct;
	
	FMeleeRuntimeDataStruct()
	:
	TraceSize(0.0f),
	HitDirection(ESpatialDirection::None),
	SlashFX(nullptr),
	HitFX(nullptr),
	HitReactionEffect(nullptr),
	HitReactionTag(FGameplayTag::EmptyTag)
	{}
	
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
	TArray<FHitReactionDataStruct> PossibleHitReactions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MeleeAssetData | Hit")
	TSubclassOf<UGameplayEffect> GrantHitReactionEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MeleeAssetData | Hit")
	TSubclassOf<UGameplayEffect> GrantDownedHitReactionEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MeleeAssetData | Hit", Meta = (Categories = "HitTag"))
	FGameplayTag HitReactionTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MeleeAssetData | Hit", Meta = (Categories = "HitTag"))
	FGameplayTag DownedHitReactionTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MeleeAssetData | VFX")
	TObjectPtr<UNiagaraSystem> SlashFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MeleeAssetData | VFX")
	TObjectPtr<UNiagaraSystem> HitFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MeleeAssetData | Size")
	float TraceSize = 20.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cue")
	FGameplayTag SlashEffectCueTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cue", Meta = (Categories = "GameplayCue"))
	FGameplayTag HitEffectCueTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "After Image")
	float AfterImageDistance = 200.0f;
	
	FGameplayEventData OnHitEventData;

	//Current

	UPROPERTY()
	TSubclassOf<UGameplayEffect> CurrentHitReactionEffect = nullptr;
	
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> CurrentHitFX = nullptr;
	
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> CurrentSlashFX = nullptr;

	UPROPERTY()
	FGameplayTag CurrentHitReactionTag = FGameplayTag::EmptyTag;

	UPROPERTY()
	ESpatialDirection CurrentHitDirection = ESpatialDirection::None;

	UPROPERTY()
	float CurrentTraceSize = 0.0f;

	// UPROPERTY()
	// FHitReactionStruct SelectedHitReactionStruct;

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
	void SetRuntimeMeleeData(FMeleeRuntimeDataStruct InMeleeRuntimeData);

protected:
	
	UFUNCTION()
	void OnHitAdded(FHitResult LastItem);

	FGameplayAbilityTargetDataHandle AddHitResultToTargetData(const FHitResult& LastItem);

	void SelectHitReaction(UAbilitySystemComponent* TargetASC, UCombatStateComponent* CombatStateComponent, FHitReactionDataStruct& InHitReactionStruct);
	bool GetTargetStateComponentsAndHitReaction(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FHitReactionDataStruct& InHitReactionStruct);
	
	virtual void ExecuteMeleeHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	virtual void SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FHitReactionDataStruct CurrentHitReactionStruct);
	virtual void ApplyMeleeHitEffects(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FHitReactionDataStruct CurrentHitReactionStruct);
	virtual void AddMeleeHitCues(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FHitReactionDataStruct CurrentHitReactionStruct);

	virtual void PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage) override;
	virtual void ExtractMeleeAssetProperties(TObjectPtr<UMeleeAbilityDataAsset> MeleeAsset);

	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

};
