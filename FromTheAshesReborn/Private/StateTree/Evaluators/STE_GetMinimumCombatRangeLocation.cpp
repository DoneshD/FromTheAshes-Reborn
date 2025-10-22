#include "StateTree/Evaluators/STE_GetMinimumCombatRangeLocation.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/KismetMathLibrary.h"

void FStateTreeEvaluator_GetMinimumCombatRangeLocation::TreeStart(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorCommonBase::TreeStart(Context);
	
}

void FStateTreeEvaluator_GetMinimumCombatRangeLocation::TreeStop(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorCommonBase::TreeStop(Context);
}

void FStateTreeEvaluator_GetMinimumCombatRangeLocation::Tick(FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FStateTreeEvaluatorCommonBase::Tick(Context, DeltaTime);
	
	FInstanceDataType* InstanceData = &Context.GetInstanceData<FInstanceDataType>(*this);
	
	if(!InstanceData->TargetActor || !InstanceData->TargetActor->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("FStateTreeEvaluator_GetMinimumCombatRangeLocation::TreeStart - Invalid TargetActor"))
		return;
	}
	
	if(!InstanceData->OwningActor || !InstanceData->OwningActor->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("FStateTreeEvaluator_GetMinimumCombatRangeLocation::TreeStart - Invalid OwningActor"))
		return;
	}
	
	FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(InstanceData->TargetActor->GetActorLocation(), InstanceData->OwningActor->GetActorLocation());
	
	InstanceData->MinimumLocation = InstanceData->TargetActor->GetActorLocation() + (Direction * InstanceData->DistanceToTarget);
	
}
