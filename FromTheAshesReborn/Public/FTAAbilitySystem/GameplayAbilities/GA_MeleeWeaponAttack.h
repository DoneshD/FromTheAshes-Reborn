
#pragma once

#include "CoreMinimal.h"
#include "GA_FromEquipment.h"
#include "GA_MeleeWeaponAttack.generated.h"


class UFTAAT_PlayMontageAndWaitForEvent;

UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeWeaponAttack : public UGA_FromEquipment
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UFTAAT_PlayMontageAndWaitForEvent> Task;

public:

	UGA_MeleeWeaponAttack(const FObjectInitializer& = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="FTA|Ability")
	UMeleeWeaponInstance* GetMeleeWeaponInstance() const;

	//~UGameplayAbility interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~End of UGameplayAbility interface

protected:
	
	// static int32 FindFirstPawnHitResult(const TArray<FHitResult>& HitResults);
	//
	// // Does a single weapon trace, either sweeping or ray depending on if SweepRadius is above zero
	// FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, OUT TArray<FHitResult>& OutHitResults) const;
	//
	// // Wrapper around WeaponTrace to handle trying to do a ray trace before falling back to a sweep trace if there were no hits and SweepRadius is above zero 
	// FHitResult DoSingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, OUT TArray<FHitResult>& OutHits) const;
	//
	// // Traces all of the bullets in a single cartridge
	// void TraceBulletsInCartridge(const FRangedWeaponFiringInput& InputData, OUT TArray<FHitResult>& OutHits);
	//
	// virtual void AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const;
	//
	// // Determine the trace channel to use for the weapon trace(s)
	// virtual ECollisionChannel DetermineTraceChannel(FCollisionQueryParams& TraceParams, bool bIsSimulated) const;
	//
	// void PerformLocalTargeting(OUT TArray<FHitResult>& OutHits);
	//
	// FVector GetWeaponTargetingSourceLocation() const;
	// FTransform GetTargetingTransform(APawn* SourcePawn, ELyraAbilityTargetingSource Source) const;

	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

// 	UFUNCTION(BlueprintCallable)
// 	void StartRangedWeaponTargeting();
//
// 	// Called when target data is ready
// 	UFUNCTION(BlueprintImplementableEvent)
// 	void OnRangedWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);
//
// private:
// 	FDelegateHandle OnTargetDataReadyCallbackDelegateHandle;
};
