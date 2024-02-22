#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"

class FROMTHEASHESREBORN_API TimelineHelper
{
public:
	static UTimelineComponent* 
		CreateTimeline(UTimelineComponent* Timeline, AActor* Target, UCurveFloat* LerpCurve, FName TimelineName, FName InterpFunctionName, FName FinishedFunctionName);
};
