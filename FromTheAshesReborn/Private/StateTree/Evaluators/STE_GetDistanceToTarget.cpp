#include "StateTree/Evaluators/STE_GetDistanceToTarget.h"

#include "StateTreeExecutionContext.h"

void FStateTreeEvaluator_GetDistanceToTarget::TreeStart(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorCommonBase::TreeStart(Context);
	
}

void FStateTreeEvaluator_GetDistanceToTarget::TreeStop(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorCommonBase::TreeStop(Context);
	
}

void FStateTreeEvaluator_GetDistanceToTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FStateTreeEvaluatorCommonBase::Tick(Context, DeltaTime);
	
	FInstanceDataType* InstanceData = &Context.GetInstanceData<FInstanceDataType>(*this);
	if(!InstanceData->OwningActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Owning Actor is null"))
		return;
	}

	if(!InstanceData->TargetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Target Actor is null"))
		return;
	}

	InstanceData->DistanceToTarget = FVector::Dist(InstanceData->TargetActor->GetActorLocation(), InstanceData->OwningActor->GetActorLocation());
	
}
