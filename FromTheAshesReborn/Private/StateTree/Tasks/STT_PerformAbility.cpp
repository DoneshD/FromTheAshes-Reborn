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
	if (IsTaskFinished)
	{
		IsTaskFinished = false;
		return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Running;
}

void FStateTreeTask_PerformAbility::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	// AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	// UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor);

	if (ASC && TagDelegateHandle.IsValid())
	{
		ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StateTreeTag.Task.Ability.Finished"), EGameplayTagEventType::NewOrRemoved)
			.Remove(TagDelegateHandle);
	}

	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}

EStateTreeRunStatus FStateTreeTask_PerformAbility::PerformAbility(const FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.InputActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Input Actor is null"))
		return EStateTreeRunStatus::Failed;
	}
	
	FGameplayEventData EventData;
	EventData.Instigator = InstanceData.InputActor;
	EventData.EventTag = FGameplayTag::RequestGameplayTag("StateTreeTag.Event.Ability.One");

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InstanceData.InputActor, EventData.EventTag, EventData);
	ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.InputActor);
	
	if (ASC)
	{
		FinishedTag = FGameplayTag::RequestGameplayTag("StateTreeTag.Task.Ability.Finished");
		TagDelegateHandle = ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StateTreeTag.Task.Ability.Finished"), EGameplayTagEventType::NewOrRemoved).AddRaw(this, &FStateTreeTask_PerformAbility::FinishTask);
	}
	
	return EStateTreeRunStatus::Running;
}

void FStateTreeTask_PerformAbility::FinishTask(FGameplayTag Tag, int32 NewCount) const
{
	if (Tag == FinishedTag && NewCount > 0)
	{
		ASC->RemoveLooseGameplayTag(FinishedTag);
		IsTaskFinished = true;
	}
	else
	{
		
	}
}
