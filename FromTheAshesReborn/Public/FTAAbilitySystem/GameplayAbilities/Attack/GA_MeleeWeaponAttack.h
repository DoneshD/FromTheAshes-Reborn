
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
struct FMeleeAttackTypes
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee Attack Types")
	TArray<TObjectPtr<UFTAAbilityDataAsset>> NormalAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee Attack Types")
	TArray<TObjectPtr<UFTAAbilityDataAsset>> PauseAttacks;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee Attack Types")
	TArray<TObjectPtr<UFTAAbilityDataAsset>> SpecialAttacks;
	
};

UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeWeaponAttack : public UGA_FromEquipment
{
	GENERATED_BODY()

protected:

	TObjectPtr<AWeaponActorBase> MeleeWeaponActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee Attack")
	TArray<TObjectPtr<UMeleeAbilityDataAsset>> MeleeAttackAssets;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee Attack")
	TSubclassOf<UGameplayEffect> MeleeAttackDamageEffect;

	TObjectPtr<AFTACharacter> FTAChar;

	TObjectPtr<UComboManagerComponent> ComboManagerComponent;


protected:

	UGA_MeleeWeaponAttack(const FObjectInitializer& = FObjectInitializer::Get());

	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void MotionWarpToTarget() override;
	
public:
	UFUNCTION(BlueprintCallable, Category="FTAAbility")
	void ResetMeleeAttack();
	
	UFUNCTION(BlueprintCallable, Category="FTAAbility")
	void PerformMeleeAttack(TArray<UMeleeAbilityDataAsset*> MeleeAttackDataAssets);

	UFUNCTION(BlueprintCallable, Category="FTAAbility")
	UMeleeWeaponInstance* GetMeleeWeaponInstance() const;

protected:
	struct FMeleeWeaponTraceData
	{
		FVector StartTrace;
		FVector EndTrace;

		UMeleeWeaponInstance* WeaponData = nullptr;

		FMeleeWeaponTraceData() : StartTrace(ForceInitToZero), EndTrace(ForceInitToZero){}
		
	};
	
	void OnMeleeWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);
	
	UFUNCTION()
	void OnHitAdded(FHitResult LastItem);

	virtual void PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage) override;

	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

};
