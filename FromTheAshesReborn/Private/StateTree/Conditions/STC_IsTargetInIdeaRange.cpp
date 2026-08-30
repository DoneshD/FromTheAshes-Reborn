#include "StateTree/Conditions/STC_IsTargetInIdeaRange.h"
#include "StateTreeExecutionContext.h"

bool FStateTreeCondition_IsTargetInIdeaRange::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	UE_LOG(LogTemp, Warning, TEXT("DistanceBetweenActors: %f"), InstanceData.DistanceBetweenActors);
	UE_LOG(LogTemp, Warning, TEXT("AcceptableRange: %f"), InstanceData.AcceptableRange);

	if (InstanceData.DistanceBetweenActors >= InstanceData.AcceptableRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("IsTargetInIdeaRange: TRUE | %f >= %f"),
			InstanceData.DistanceBetweenActors,
			InstanceData.AcceptableRange);

		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("IsTargetInIdeaRange: FALSE | %f < %f"),
		InstanceData.DistanceBetweenActors,
		InstanceData.AcceptableRange);

	return false;
}
