#pragma once

#include "CoreMinimal.h"
#include "FTAGameplayAbility.h"
#include "GA_GroundedMeleeAttack.generated.h"


class UMeleeAttackDataAsset;

UCLASS()
class FROMTHEASHESREBORN_API UGA_GroundedMeleeAttack : public UFTAGameplayAbility
{
	GENERATED_BODY()

protected:

	UGA_GroundedMeleeAttack();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TArray<EAbilityInputID> AbilityInputIDs;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UMeleeAttackDataAsset>> LightAttacks;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UMeleeAttackDataAsset>> HeavyAttacks;
	
	int32 CurrentComboIndex;

	int32 MaxComboNum;

	bool IsComboFinisher;
	bool IsComboQueued;
	bool IsComboWindowOpen;

	UPROPERTY(EditAnywhere)
	FTimerHandle FComboWindowTimer;
	
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
	
	UAnimMontage* AttackMontageToPlay;
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
	void OnComboWindowOpen(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnComboWindowClose(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	void WaitForComboWindow();

	void ProceedToNextCombo();

	void CheckForInput();

	void PlayAttackMontage();
	
};
