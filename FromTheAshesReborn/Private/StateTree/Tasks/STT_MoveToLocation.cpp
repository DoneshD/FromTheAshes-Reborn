#include "StateTree/Tasks/STT_MoveToLocation.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Navigation/PathFollowingComponent.h"

EStateTreeRunStatus FStateTreeTask_MoveToLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("Input AIController is null"))
		return EStateTreeRunStatus::Failed;
	}

	FAIMoveRequest MoveRequest;
	
	MoveRequest.SetGoalLocation(InstanceData.Location);
	MoveRequest.SetAcceptanceRadius(InstanceData.AcceptableRadius);
	
	PathFollowingRequestResult = InstanceData.AIController->MoveTo(MoveRequest);
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FStateTreeTask_MoveToLocation::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FStateTreeTaskCommonBase::Tick(Context, DeltaTime);

	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("Input AIController is null"))
		return EStateTreeRunStatus::Failed;
	}

	if(!InstanceData.AIController->GetPathFollowingComponent() || !InstanceData.AIController->GetPathFollowingComponent()->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("Input PathFollowingComponent is null"))
		return EStateTreeRunStatus::Failed;
	}

	EPathFollowingStatus::Type MoveStatus = InstanceData.AIController->GetPathFollowingComponent()->GetStatus();

	switch (MoveStatus)
	{
	case EPathFollowingStatus::Idle:
		return EStateTreeRunStatus::Succeeded;

	case EPathFollowingStatus::Paused:
		UE_LOG(LogTemp, Error, TEXT("Move Status Paused"));
		return EStateTreeRunStatus::Running;

	case EPathFollowingStatus::Moving:
		return EStateTreeRunStatus::Running;

	default:
		UE_LOG(LogTemp, Error, TEXT("Move Status invalid state"));
		return EStateTreeRunStatus::Failed;
	}
	
}

void FStateTreeTask_MoveToLocation::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}
