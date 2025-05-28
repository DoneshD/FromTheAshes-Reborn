
#pragma once

#include "CoreMinimal.h"
#include "../GA_FromEquipment.h"
#include "GA_MeleeWeaponAttack.generated.h"


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

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<AWeaponActorBase> MeleeWeaponActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee Attacka")
	FMeleeAttackForms MeleeAttackAssets;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee Effects")
	TSubclassOf<UGameplayEffect> MeleeAttackDamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee Effects")
	TSubclassOf<UGameplayEffect> MeleeAttackHitReactionEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee Effects", Meta = (Categories = "HitTag"))
	FGameplayTag GameplayEventTagOnHit;

	FGameplayEventData OnHitEventData;

	TObjectPtr<AFTACharacter> FTAChar;

	TObjectPtr<UComboManagerComponent> ComboManagerComponent;

	TObjectPtr<UMeleeAbilityDataAsset> MeleeAbilityAsset;


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
	virtual void SendMeleeHitGameplayEvents(const FHitResult& LastItem);
	virtual void ApplyMeleeHitEffects(const FGameplayAbilityTargetDataHandle& TargetData);

	virtual void PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage) override;

	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

};
