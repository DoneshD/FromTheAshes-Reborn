#include "StateTree/Tasks/STT_RangedAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

EStateTreeRunStatus FStateTreeTask_RangedAttack::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return ActivateRangedAttack(Context);
}

EStateTreeRunStatus FStateTreeTask_RangedAttack::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (IsTaskFinished)
	{
		IsTaskFinished = false;
		return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Running;
}

void FStateTreeTask_RangedAttack::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	// AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	// UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor);

	if (ASC && TagDelegateHandle.IsValid())
	{
		ASC->RegisterGameplayTagEvent(InstanceData.StateTreeFinishedTag, EGameplayTagEventType::NewOrRemoved)
			.Remove(TagDelegateHandle);
	}

	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}

EStateTreeRunStatus FStateTreeTask_RangedAttack::ActivateRangedAttack(const FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.InputActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Input Actor is null"))
		return EStateTreeRunStatus::Failed;
	}
	
	FGameplayEventData EventData;
	EventData.Instigator = InstanceData.InputActor;
	EventData.EventTag = FGameplayTag::RequestGameplayTag("StateTreeEvent.Attack.Heavy");

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InstanceData.InputActor, EventData.EventTag, EventData);
	ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.InputActor);
	
	if (ASC)
	{
		FinishedTag = InstanceData.StateTreeFinishedTag;
		TagDelegateHandle = ASC->RegisterGameplayTagEvent(InstanceData.StateTreeFinishedTag, EGameplayTagEventType::NewOrRemoved).AddRaw(this, &FStateTreeTask_RangedAttack::FinishTask);
	}
	
	return EStateTreeRunStatus::Running;
}

void FStateTreeTask_RangedAttack::FinishTask(FGameplayTag Tag, int32 NewCount) const
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