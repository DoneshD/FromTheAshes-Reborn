﻿#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "FTAAT_WaitReceiveDamage.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWaitReceiveDamageDelegate, class UFTAAbilitySystemComponent*, SourceASC, float, UnmitigatedDamage, float, MitigatedDamage);


UCLASS()
class FROMTHEASHESREBORN_API UFTAAT_WaitReceiveDamage : public UAbilityTask
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(BlueprintAssignable)
	FWaitReceiveDamageDelegate OnDamage;

	virtual void Activate() override;

	UFUNCTION()
	void OnDamageReceived(class UFTAAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UFTAAT_WaitReceiveDamage* WaitReceiveDamage(UGameplayAbility* OwningAbility, bool TriggerOnce);

protected:
	
	bool TriggerOnce;

	virtual void OnDestroy(bool AbilityIsEnding) override;
};
