﻿#pragma once

#include "CoreMinimal.h"
#include "FTAGameplayAbility.h"
#include "GA_GroundedMeleeAttack.generated.h"


class UMeleeAttackDataAsset;
class UFTAAT_PlayMontageAndWaitForEvent;

UCLASS()
class FROMTHEASHESREBORN_API UGA_GroundedMeleeAttack : public UFTAGameplayAbility
{
	GENERATED_BODY()

protected:

	UGA_GroundedMeleeAttack();
	
	int32 CurrentComboIndex;

	bool IsComboFinisher;
	bool IsComboQueued;
	bool IsComboWindowOpen;
	
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
	
	TObjectPtr<UAnimMontage> AttackMontageToPlay;

	FTimerHandle FComboWindowTimer;

	UPROPERTY(BlueprintReadWrite)
	UFTAAT_PlayMontageAndWaitForEvent* Task;
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
	void WaitForComboWindow();

	void ProceedToNextCombo();

	void CheckForInput();

	virtual void PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage);

	void ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
};