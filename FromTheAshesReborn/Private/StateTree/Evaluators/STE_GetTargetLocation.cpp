#include "StateTree/Evaluators/STE_GetTargetLocation.h"

#include "StateTreeExecutionContext.h"

void FStateTreeEvaluator_GetTargetLocation::TreeStart(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorCommonBase::TreeStart(Context);
}

void FStateTreeEvaluator_GetTargetLocation::TreeStop(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorCommonBase::TreeStop(Context);
}

void FStateTreeEvaluator_GetTargetLocation::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FStateTreeEvaluatorCommonBase::Tick(Context, DeltaTime);

	FInstanceDataType* InstanceData = &Context.GetInstanceData<FInstanceDataType>(*this);
	
	if(!InstanceData->TargetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Target Actor is null"))
		return;
	}

	InstanceData->TargetLocation = InstanceData->TargetActor->GetActorLocation();

}
