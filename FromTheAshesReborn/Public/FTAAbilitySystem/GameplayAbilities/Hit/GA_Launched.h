#pragma once

#include "CoreMinimal.h"
#include "GA_ReceiveHit.h"
#include "GA_Launched.generated.h"


class UAT_LaunchCharacterAndWait;
class ULauncherComponent;

UCLASS()
class FROMTHEASHESREBORN_API UGA_Launched : public UGA_ReceiveHit
{
	GENERATED_BODY()

protected:

	TObjectPtr<UAT_LaunchCharacterAndWait> LaunchTask;

public:

	UGA_Launched();

	virtual void OnAbilityTick(float DeltaTime) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnLaunchComplete();
};

