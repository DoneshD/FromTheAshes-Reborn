#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackTargetInterface.generated.h"

UINTERFACE(MinimalAPI)
class UAttackTargetInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IAttackTargetInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual int GetMaxAttackersCount() = 0;
};
