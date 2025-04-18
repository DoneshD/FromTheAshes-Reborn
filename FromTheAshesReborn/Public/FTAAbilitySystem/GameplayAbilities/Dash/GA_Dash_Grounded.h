﻿#pragma once

#include "CoreMinimal.h"
#include "GA_Dash.h"
#include "GA_Dash_Grounded.generated.h"


UCLASS()
class FROMTHEASHESREBORN_API UGA_Dash_Grounded : public UGA_Dash
{
	GENERATED_BODY()

	

protected:
	
	UGA_Dash_Grounded();

	virtual void OnAbilityTick(float DeltaTime) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void DashLocationReached() override;

	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
};
