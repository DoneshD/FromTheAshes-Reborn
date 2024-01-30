// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageSystem/DamageInfo.h"
#include "DamagableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamagableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FROMTHEASHESREBORN_API IDamagableInterface
{
	GENERATED_BODY()

public:

	bool WithinParryRange = false;

	UFUNCTION()
	virtual float NativeGetCurrentHealth() = 0;

	UFUNCTION()
	virtual float NativeGetMaxHealth() = 0;

	UFUNCTION()
	virtual bool NativeIsDead() = 0;

	UFUNCTION()
	virtual float NativeHeal(float HealAmount) = 0;

	UFUNCTION()
	virtual bool NativeTakeDamage(FDamageInfo DamageInfo) = 0;

	UFUNCTION()
	virtual bool ReserveAttackToken(int Amount) = 0;

	UFUNCTION()
	virtual void ReturnAttackToken(int Amount) = 0;
};
