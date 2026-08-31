#include "StateTree/Conditions/STC_IsTargetInIdeaRange.h"
#include "StateTreeExecutionContext.h"

bool FStateTreeCondition_IsTargetInIdeaRange::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.DistanceBetweenActors >= InstanceData.AcceptableRange)
	{
		return true;
	}
	
	return false;
}
