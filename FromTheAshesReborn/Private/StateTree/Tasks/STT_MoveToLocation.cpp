#include "StateTree/Tasks/STT_MoveToLocation.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

EStateTreeRunStatus FStateTreeTask_MoveToLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.InputActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Input Actor is null"))
		return EStateTreeRunStatus::Failed;
	}

	EPathFollowingRequestResult::Type PathFollowingRequestResult = InstanceData.AIController->MoveToLocation(InstanceData.Location, InstanceData.AcceptableRadius);
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FStateTreeTask_MoveToLocation::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return FStateTreeTaskCommonBase::Tick(Context, DeltaTime);
}

void FStateTreeTask_MoveToLocation::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}
