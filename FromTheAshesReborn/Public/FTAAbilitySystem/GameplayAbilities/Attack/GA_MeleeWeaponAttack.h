
#pragma once

#include "CoreMinimal.h"
#include "../GA_FromEquipment.h"
#include "GA_MeleeWeaponAttack.generated.h"

class UNiagaraSystem;
class UComboManagerComponent;
class UMeleeAbilityDataAsset;
class UFTAAbilityDataAsset;
class UMeleeAttackDataAsset;
class AWeaponActorBase;
class UMeleeWeaponInstance;
class UFTAAT_PlayMontageAndWaitForEvent;


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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool NonMontageAttack = false;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<AWeaponActorBase> MeleeWeaponActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attacks")
	FMeleeAttackForms MeleeAttackAssets;

	UPROPERTY(BlueprintReadWrite, Category = "Attacks")
	TObjectPtr<UMeleeAbilityDataAsset> MeleeAbilityAsset;

	UPROPERTY(EditDefaultsOnly, Category = "Attacks")
	float BoxHalfSize;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GE")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GE")
	TSubclassOf<UGameplayEffect> HitReactionEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GE", Meta = (Categories = "HitTag"))
	FGameplayTag HitReactionTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	TObjectPtr<UNiagaraSystem> SlashFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	FGameplayTag SlashVfxCueTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	TObjectPtr<UNiagaraSystem> HitFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX", Meta = (Categories = "GameplayCue"))
	FGameplayTag HitVfxCueTag;
	
	FGameplayEventData OnHitEventData;

	TObjectPtr<AFTACharacter> FTAChar;

	TObjectPtr<UComboManagerComponent> ComboManagerComponent;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> CurrentHitReactionEffect = nullptr;
	
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> CurrentHitFX = nullptr;
	
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> CurrentSlashFX = nullptr;

	UPROPERTY()
	FGameplayTag CurrentHitReactionTag = FGameplayTag::EmptyTag;


protected:

	UGA_MeleeWeaponAttack(const FObjectInitializer& = FObjectInitializer::Get());

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void MotionWarpToTarget() override;
	
public:
	UFUNCTION(BlueprintCallable, Category="FTAAbility")
	void ResetMeleeAttack();

	UFUNCTION(BlueprintCallable, Category="FTAAbility")
	void PerformMeleeAttack(FMeleeAttackForms& MeleeAttackDataAssets);

	UFUNCTION(BlueprintCallable, Category="FTAAbility")
	UMeleeWeaponInstance* GetMeleeWeaponInstance() const;

protected:
	
	UFUNCTION()
	void OnHitAdded(FHitResult LastItem);

	FGameplayAbilityTargetDataHandle AddHitResultToTargetData(const FHitResult& LastItem);
	virtual void ExecuteMeleeHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	virtual void SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	virtual void ApplyMeleeHitEffects(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	virtual void AddMeleeHitCues(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	virtual void PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage) override;

	virtual void ExtractMeleeAssetProperties(TObjectPtr<UMeleeAbilityDataAsset> MeleeAsset);

	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

};
