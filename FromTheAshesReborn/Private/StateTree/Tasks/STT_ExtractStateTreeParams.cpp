#include "StateTree/Tasks/STT_ExtractStateTreeParams.h"

#include "StateTreeExecutionContext.h"
#include "DataAsset/AICombatParameters.h"

EStateTreeRunStatus FStateTreeTask_ExtractStateTreeParams::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType* InstanceData = &Context.GetInstanceData<FInstanceDataType>(*this);
	// const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if(!InstanceData->CombatParameters)
	{
		UE_LOG(LogTemp, Error, TEXT("FStateTreeTask_ExtractStateTreeParams::EnterState - CombatParameters is null"))
		return EStateTreeRunStatus::Failed;

	}

	InstanceData->OutCombatParameters = InstanceData->CombatParameters;
	
	return EStateTreeRunStatus::Succeeded;
	
}

EStateTreeRunStatus FStateTreeTask_ExtractStateTreeParams::Tick(FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	return FStateTreeTaskCommonBase::Tick(Context, DeltaTime);
}

void FStateTreeTask_ExtractStateTreeParams::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}