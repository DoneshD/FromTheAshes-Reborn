#pragma once

#include "CoreMinimal.h"
#include "FTAGameplayAbility.h"
#include "GA_GroundedDash.generated.h"

class UDashDataAsset;
class UFTAAT_PlayMontageAndWaitForEvent;

UCLASS()
class FROMTHEASHESREBORN_API UGA_GroundedDash : public UFTAGameplayAbility
{
	GENERATED_BODY()

protected:

	UGA_GroundedDash();

	FGameplayTag DashInput = FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Light"));
	FGameplayTag DashWindowTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.ComboWindow.Open.Light"));

	FTimerHandle FDashComboWindowTimer;
	
	TObjectPtr<UAnimMontage> DashMontageToPlay;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UDashDataAsset>> DashDataAssets;
	
	UPROPERTY(BlueprintReadWrite)
	UFTAAT_PlayMontageAndWaitForEvent* Task;

public:

	void ResetDash();
	
	virtual void DashWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	void DashWindowTagOpen();
	
	void PerformDash(TArray<TObjectPtr<UDashDataAsset>> SelectDashDataAssets);
	
	virtual void PlayDashMontage(TObjectPtr<UAnimMontage> DashMontage);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	

	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};
