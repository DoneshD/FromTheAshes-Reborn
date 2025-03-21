﻿#pragma once

#include "CoreMinimal.h"
#include "FTAGameplayAbility.h"
#include "GA_LockOn.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UGA_LockOn : public UFTAGameplayAbility
{
	GENERATED_BODY()

protected:

	bool IsTargeting = false;

	TObjectPtr<AActor> LockedOnTarget;

protected:
	
	UGA_LockOn();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

};
