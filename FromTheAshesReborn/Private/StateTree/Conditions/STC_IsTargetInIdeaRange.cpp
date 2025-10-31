#include "StateTree/Conditions/STC_IsTargetInIdeaRange.h"
#include "StateTreeExecutionContext.h"

bool FStateTreeCondition_IsTargetInIdeaRange::TestCondition(FStateTreeExecutionContext& Context) const
{
	//TODO: Returning false for some reason
	/*if(!FStateTreeConditionCommonBase::TestCondition(Context))
	{
		return false;
	}*/

	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if(InstanceData.DistanceBetweenActors >= InstanceData.AcceptableRadius)
	{
		return true;
	}
	
	return false;
	
}
