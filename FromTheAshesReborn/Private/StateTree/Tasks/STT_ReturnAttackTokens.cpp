#include "StateTree/Tasks/STT_ReturnAttackTokens.h"

EStateTreeRunStatus FStateTreeTask_ReturnAttackTokens::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus FStateTreeTask_ReturnAttackTokens::Tick(FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	return FStateTreeTaskCommonBase::Tick(Context, DeltaTime);
}

void FStateTreeTask_ReturnAttackTokens::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}
