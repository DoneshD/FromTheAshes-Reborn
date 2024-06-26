#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DashingCombatantInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDashingCombatantInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IDashingCombatantInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual void DashWarpToTarget(bool HasInput) = 0;

	UFUNCTION()
	virtual void ResetDashWarpToTarget() = 0;
};
