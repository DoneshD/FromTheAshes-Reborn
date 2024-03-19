#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PositionalWarpingInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPositionalWarpingInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IPositionalWarpingInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual void UpdateWarpTargetPostion(FMotionWarpingTarget MotionWarpingTargetParams) = 0;

	UFUNCTION()
	virtual void ResetWarpTargetPostion(FName TargetName) = 0;

	virtual TObjectPtr<UArrowComponent> GetPositionalArrow(EFacingDirection HitDirection) = 0;

	virtual TObjectPtr<UArrowComponent> GetLeftArrowNeighbor(TObjectPtr<UArrowComponent> Arrow) = 0;

	virtual TObjectPtr<UArrowComponent> GetRightArrowNeighbor(TObjectPtr<UArrowComponent> Arrow) = 0;

	UFUNCTION()
	virtual EFacingDirection GetFacingDirection(FVector HitLocation) = 0;
};
