
#pragma once

#include "CoreMinimal.h"
#include "GA_FromEquipment.h"
#include "GA_MeleeWeaponAttack.generated.h"


class UMeleeAttackDataAsset;
class AWeaponActorBase;
class UMeleeWeaponInstance;
class UFTAAT_PlayMontageAndWaitForEvent;

UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeWeaponAttack : public UGA_FromEquipment
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UFTAAT_PlayMontageAndWaitForEvent> Task;

	TObjectPtr<AWeaponActorBase> MeleeWeaponActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee Attack Data")
	TArray<TObjectPtr<UMeleeAttackDataAsset>> MeleeAttackAssets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee Attack Data")
	TSubclassOf<UGameplayEffect> MeleeAttackDamageEffect;

public:

	UGA_MeleeWeaponAttack(const FObjectInitializer& = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="FTA|Ability")
	UMeleeWeaponInstance* GetMeleeWeaponInstance() const;

	//~UGameplayAbility interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~End of UGameplayAbility interface

	TObjectPtr<UAnimMontage> AttackMontageToPlay;

	UFUNCTION(BlueprintCallable, Category="FTA|Ability")
	void ResetMeleeAttack();
	
	bool FindMatchingTagContainer(const TArray<UMeleeAttackDataAsset*>& MeleeAttackDataAssets,
		TObjectPtr<UMeleeAttackDataAsset>& OutMatchingDataAsset);
	
	UFUNCTION(BlueprintCallable, Category="FTA|Ability")
	void PerformMeleeAttack(TArray<UMeleeAttackDataAsset*> MeleeAttackDataAssets);

	// UFUNCTION()
	void PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage);

protected:
	struct FMeleeWeaponTraceData
	{
		// Start of the trace
		FVector StartTrace;

		// End of the trace if aim were perfect
		FVector EndTrace;

		// The weapon instance / source of weapon data
		UMeleeWeaponInstance* WeaponData = nullptr;

		FMeleeWeaponTraceData()
			: StartTrace(ForceInitToZero)
			, EndTrace(ForceInitToZero)
		{
		}
	};
	
	UFUNCTION(BlueprintCallable)
	void StartMeleeWeaponTargeting();

	UFUNCTION(BlueprintCallable)
	void EndMeleeWeaponTargeting();
	
	void OnMeleeWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);
	
	UFUNCTION()
	void OnHitAdded(FHitResult LastItem);

	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);

};
