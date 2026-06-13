#pragma once

#include "CoreMinimal.h"
#include "../FTAGameplayAbility.h"
#include "DataAsset/DashAbilityDataAsset.h"
#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"
#include "GA_Dash.generated.h"

class UDashAbilityDataAsset;
class UFTAAbilityDataAsset;

UCLASS()
class FROMTHEASHESREBORN_API UGA_Dash : public UFTAGameplayAbility
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	TArray<TObjectPtr<UDashAbilityDataAsset>> DashAbilityAssets;

	ELockOnInputOrientationDirection InputDirection = ELockOnInputOrientationDirection::Forward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	TObjectPtr<UDashAbilityDataAsset> DefaultDashData;
	
	UPROPERTY(BlueprintReadWrite, Category = "Dash")
	TObjectPtr<UDashAbilityDataAsset> CurrentDashData;


protected:

	UGA_Dash();

	virtual void OnAbilityTick(float DeltaTime) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage) override;
	
	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

	virtual UFTAAbilityDataAsset* SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets) override;
	virtual void ExtractAssetProperties(UFTAAbilityDataAsset* InAbilityAsset) override;
	virtual void SetRuntimeAbilityData(UFTAAbilityDataAsset* InAbilityRuntimeData) override;
	
	virtual void PerformAbility(UFTAAbilityDataAsset* InAbilityAsset) override;
	

};
