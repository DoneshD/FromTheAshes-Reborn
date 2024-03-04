// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MotionWarpingInterface.generated.h"

// This class does not need to be modified.
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

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void UpdateWarpTarget(FMotionWarpingTarget& MotionWarpingTargetParams) = 0;

	virtual void ResetWarpTarget() = 0;

	virtual TObjectPtr<UArrowComponent> GetPositionArrow(EHitReactionDirection HitDirection) = 0;
};
