#include "StateTree/Tasks/STT_ReceiveHit.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FStateTreeTask_ReceiveHit::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.InputActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Input Actor is null"))
		return EStateTreeRunStatus::Failed;
	}

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.InputActor))
	{
		FinishedTag = InstanceData.StateTreeFinishedTag;
		TagDelegateHandle = ASC->RegisterGameplayTagEvent(InstanceData.StateTreeFinishedTag, EGameplayTagEventType::NewOrRemoved).AddRaw(this, &FStateTreeTask_ReceiveHit::FinishTask);
	}
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FStateTreeTask_ReceiveHit::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (IsTaskFinished)
	{
		IsTaskFinished = false;
		return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Running;
}

void FStateTreeTask_ReceiveHit::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.InputActor))
	{
		ASC->UnregisterGameplayTagEvent(TagDelegateHandle, FinishedTag);
	}

	UE_LOG(LogTemp, Error, TEXT("Exit hit"));
	
	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}

void FStateTreeTask_ReceiveHit::FinishTask(FGameplayTag Tag, int32 NewCount) const
{
	if (Tag == FinishedTag && NewCount > 0)
	{
		IsTaskFinished = true;
	}
}
