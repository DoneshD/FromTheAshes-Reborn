#include "StateTree/Tasks/STT_MeleeWeaponAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

EStateTreeRunStatus FStateTreeTask_MeleeWeaponAttack::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return ActivateMeleeAttack(Context);
}

EStateTreeRunStatus FStateTreeTask_MeleeWeaponAttack::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (IsTaskFinished)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Running;
}

void FStateTreeTask_MeleeWeaponAttack::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor);

	if (ASC && TagDelegateHandle.IsValid())
	{
		ASC->RegisterGameplayTagEvent(InstanceData.StateTreeFinishedTag, EGameplayTagEventType::NewOrRemoved)
			.Remove(TagDelegateHandle);
	}

	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}

EStateTreeRunStatus FStateTreeTask_MeleeWeaponAttack::ActivateMeleeAttack(const FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.InputActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Input Actor is null"))
		return EStateTreeRunStatus::Failed;
	}
	
	FGameplayEventData EventData;
	EventData.Instigator = InstanceData.InputActor;
	EventData.EventTag = FGameplayTag::RequestGameplayTag("TestTag.Tag9");

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InstanceData.InputActor, EventData.EventTag, EventData);

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.InputActor))
	{
		FinishedTag = InstanceData.StateTreeFinishedTag;
		TagDelegateHandle = ASC->RegisterGameplayTagEvent(InstanceData.StateTreeFinishedTag, EGameplayTagEventType::NewOrRemoved).AddRaw(this, &FStateTreeTask_MeleeWeaponAttack::FinishTask);
	}
	
	return EStateTreeRunStatus::Running;
}

void FStateTreeTask_MeleeWeaponAttack::FinishTask(FGameplayTag Tag, int32 NewCount) const
{
	if (Tag == FinishedTag && NewCount > 0)
	{
		IsTaskFinished = true;
	}
}
