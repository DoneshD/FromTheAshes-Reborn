#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageSystem/DamageInfo.h"
#include "DamagableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDamagableInterface : public UInterface
{
	GENERATED_BODY()
};


class FROMTHEASHESREBORN_API IDamagableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual float GetCurrentHealth() = 0;

	UFUNCTION()
	virtual float GetMaxHealth() = 0;

	UFUNCTION()
	virtual bool IsDead() = 0;

	UFUNCTION()
	virtual float Heal(float HealAmount) = 0;

	UFUNCTION()
	virtual bool TakeDamage(FDamageInfo DamageInfo) = 0;

	UFUNCTION()
	virtual bool ReserveAttackToken(int Amount) = 0;

	UFUNCTION()
	virtual void ReturnAttackToken(int Amount) = 0;
};
