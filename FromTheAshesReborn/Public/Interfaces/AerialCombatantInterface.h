#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AerialCombatantInterface.generated.h"

UINTERFACE(MinimalAPI)
class UAerialCombatantInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IAerialCombatantInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual bool JumpLineTrace(FVector StartLocation, FVector EndLocation) = 0;

	UFUNCTION()
	virtual void JumpWarpToTarget() = 0;

	UFUNCTION()
	virtual void ResetJumpWarpToTarget() = 0;

};
