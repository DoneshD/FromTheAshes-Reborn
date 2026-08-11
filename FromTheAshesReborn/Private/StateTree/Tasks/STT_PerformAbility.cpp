#include "StateTree/Tasks/STT_PerformAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FStateTreeTask_PerformAbility::EnterState(FStateTreeExecutionContext& Context,
                                                              const FStateTreeTransitionResult& Transition) const
{
	return PerformAbility(Context);
}

EStateTreeRunStatus FStateTreeTask_PerformAbility::Tick(FStateTreeExecutionContext& Context,
	const float DeltaTime) const
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
	if(ASC->GetTagCount(InstanceData.TaskFinishedTag) > 0)
	{
		ASC->RemoveLooseGameplayTag(InstanceData.TaskFinishedTag);
		UE_LOG(LogTemp, Warning, TEXT("Success!"))
		return EStateTreeRunStatus::Succeeded;
	}
	
	return EStateTreeRunStatus::Running;
}

void FStateTreeTask_PerformAbility::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.InputActor)
	{
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.InputActor);
		if(ASC->GetTagCount(InstanceData.TaskFinishedTag))
		{
			ASC->RemoveLooseGameplayTag(InstanceData.TaskFinishedTag);
		}
	}

	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}

EStateTreeRunStatus FStateTreeTask_PerformAbility::PerformAbility(const FStateTreeExecutionContext& Context) const
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
	if(ASC->GetTagCount(InstanceData.TaskFinishedTag) > 0)
	{
		ASC->RemoveLooseGameplayTag(InstanceData.TaskFinishedTag);
	}
	
	FGameplayEventData EventData;
	EventData.Instigator = InstanceData.InputActor;
	EventData.EventTag = InstanceData.EventTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InstanceData.InputActor, EventData.EventTag, EventData);
	
	return EStateTreeRunStatus::Running;
}
