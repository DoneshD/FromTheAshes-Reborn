#include "StateTree/Tasks/STT_ReceiveHit.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"
#include "DataAsset/AICombatParameters.h"

EStateTreeRunStatus FStateTreeTask_ReceiveHit::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.InputActor)
	{
		UE_LOG(LogTemp, Error, TEXT("FStateTreeTask_PerformAbility::PerformAbility - Input Actor is null"))
		return EStateTreeRunStatus::Failed;
	}
	
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.InputActor);

	if(!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("FStateTreeTask_PerformAbility::PerformAbility - ASC is null"))
		return EStateTreeRunStatus::Failed;
	}
	if(ASC->GetTagCount(InstanceData.StateTreeFinishedTag) > 0)
	{
		ASC->RemoveLooseGameplayTag(InstanceData.StateTreeFinishedTag);
	}

	// InstanceData.CombatParams->AggressionStats.FinalWeight = 0.0f;
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FStateTreeTask_ReceiveHit::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.InputActor)
	{
		UE_LOG(LogTemp, Error, TEXT("FStateTreeTask_PerformAbility::PerformAbility - Input Actor is null"))
		return EStateTreeRunStatus::Failed;
	}
	
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.InputActor);

	if(!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("FStateTreeTask_PerformAbility::PerformAbility - ASC is null"))
		return EStateTreeRunStatus::Failed;
	}
	if(ASC->GetTagCount(InstanceData.StateTreeFinishedTag) > 0)
	{
		ASC->RemoveLooseGameplayTag(InstanceData.StateTreeFinishedTag);
		return EStateTreeRunStatus::Succeeded;
	}
	
	return EStateTreeRunStatus::Running;
}

void FStateTreeTask_ReceiveHit::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.InputActor)
	{
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.InputActor);
		if(ASC->GetTagCount(InstanceData.StateTreeFinishedTag))
		{
			ASC->RemoveLooseGameplayTag(InstanceData.StateTreeFinishedTag);
		}
	}

	// InstanceData.CombatParams->AggressionStats.FinalWeight = 1.0f;

	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}


