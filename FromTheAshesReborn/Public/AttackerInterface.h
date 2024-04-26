#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UAttackerInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IAttackerInterface
{
	GENERATED_BODY()

public:

	virtual void Attack(TObjectPtr<AActor> AttackTarget) = 0;

	virtual void Wait(TObjectPtr<AActor> AttackTarget) = 0;

	virtual void Retreat() = 0;

};
