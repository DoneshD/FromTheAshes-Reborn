#pragma once

#include "CoreMinimal.h"
#include "../FTAGameplayAbility.h"
#include "DataAsset/DashAbilityDataAsset.h"
#include "GA_Dash.generated.h"

class UDashAbilityDataAsset;
class UFTAAbilityDataAsset;

UCLASS()
class FROMTHEASHESREBORN_API UGA_Dash : public UFTAGameplayAbility
{
	GENERATED_BODY()

protected:

	bool IsDashing = false;

	FVector DashStartLocation;
	FVector DashEndLocation;
	
	float DashElapsedTime = 0.0f;
	float DashStartTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Ability")
	TArray<TObjectPtr<UDashAbilityDataAsset>> DashAbilityAssets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Ability")
	float DashDistance = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Ability")
	float DashDuration = 0.20f;

protected:

	UGA_Dash();

	virtual void OnAbilityTick(float DeltaTime) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void UpdateDashMovement(float DeltaTime);

	virtual void DashLocationReached();

	static bool FindMatchingDashAssetToInputDirection(const TArray<UDashAbilityDataAsset*>& AbilityDataAssets, TObjectPtr<UDashAbilityDataAsset>& OutMatchingAbilityDataAsset, ELockOnInputOrientationDirection InputDirection);

	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

};
