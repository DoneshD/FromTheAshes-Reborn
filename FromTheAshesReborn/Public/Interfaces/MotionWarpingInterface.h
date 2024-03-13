#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MotionWarpingInterface.generated.h"

UINTERFACE(MinimalAPI)
class UMotionWarpingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FROMTHEASHESREBORN_API IMotionWarpingInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual void UpdateWarpTarget(FMotionWarpingTarget& MotionWarpingTargetParams) = 0;

	UFUNCTION()
	virtual void ResetWarpTarget() = 0;

	virtual TObjectPtr<UArrowComponent> GetPositionArrow(EHitDirection HitDirection) = 0;

	UFUNCTION()
	virtual EHitDirection GetHitEnemyDirection(FVector HitLocation) = 0;
};
