﻿#pragma once

#include "CoreMinimal.h"
#include "FTAGameplayAbility.h"
#include "GA_GroundedMeleeAttack.generated.h"


class AFTAPlayerController;
class UMeleeAttackDataAsset;
class UFTAAT_PlayMontageAndWaitForEvent;

UCLASS()
class FROMTHEASHESREBORN_API UGA_GroundedMeleeAttack : public UFTAGameplayAbility
{
	GENERATED_BODY()

protected:

	UGA_GroundedMeleeAttack();

	FGameplayTag LightInput = FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Light"));
	FGameplayTag HeavyInput = FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Heavy"));
	FGameplayTag LightComboWindow = FGameplayTag::RequestGameplayTag(FName("Event.Montage.ComboWindow.Open.Light"));
	FGameplayTag HeavyComboWindow = FGameplayTag::RequestGameplayTag(FName("Event.Montage.ComboWindow.Open.Heavy"));

	FGameplayTagContainer CurrentComboTagContainer;
	int32 CurrentComboIndex = 0;

	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
	
	TObjectPtr<UAnimMontage> AttackMontageToPlay;
	
	FTimerHandle FLightComboWindowTimer;
	FTimerHandle FHeavyComboWindowTimer;
	
	FTimerHandle FComboWindowTimer;

	UPROPERTY(BlueprintReadWrite)
	UFTAAT_PlayMontageAndWaitForEvent* Task;

	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage);

	virtual void ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	void LightComboWindowOpen();
	void HeavyComboWindowOpen();

	virtual void LightComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void HeavyComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	void PerformGroundedMeleeAttack(TArray<TObjectPtr<UMeleeAttackDataAsset>> GroundedAttackDataAssets);
	void ProceedToNextCombo(int32 IDToActivate);
	void ResetGroundedMeleeAttack();

	bool FindMatchingTagContainer(const TArray<TObjectPtr<UMeleeAttackDataAsset>>& GroundedAttackDataAssets,
		TObjectPtr<UMeleeAttackDataAsset>& OutMatchingDataAsset);
};
