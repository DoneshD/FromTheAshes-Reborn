#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackTokenSystemInterface.generated.h"

UINTERFACE(MinimalAPI)
class UAttackTokenSystemInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IAttackTokenSystemInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual bool AttackStart(AActor* AttackTarget, int TokensNeeded) = 0;

	UFUNCTION()
	virtual void AttackEnd(AActor* AttackTarget) = 0;

	UFUNCTION()
	virtual bool ReserveAttackToken(int Amount) = 0;

	UFUNCTION()
	virtual void ReturnAttackToken(int Amount) = 0;

	UFUNCTION()
	virtual void StoreAttackTokens(AActor* AttackTarget, int Amount) = 0;
};
