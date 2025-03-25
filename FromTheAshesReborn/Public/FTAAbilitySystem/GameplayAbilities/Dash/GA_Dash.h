#pragma once

#include "CoreMinimal.h"
#include "../FTAGameplayAbility.h"
#include "GA_Dash.generated.h"

class UFTAAbilityDataAsset;

UCLASS()
class FROMTHEASHESREBORN_API UGA_Dash : public UFTAGameplayAbility
{
	GENERATED_BODY()

private:
	
	FVector DashTargetLocation;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	TArray<TObjectPtr<UFTAAbilityDataAsset>> DashAssets;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashWarpDistance = 400.0f;

protected:
	
	UGA_Dash();
	virtual void AbilityTickComponent() override;

public:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
