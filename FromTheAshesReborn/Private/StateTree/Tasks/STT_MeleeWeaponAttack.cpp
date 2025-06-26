#include "StateTree/Tasks/STT_MeleeWeaponAttack.h"
#include "GameFramework/Actor.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FStateTreeTask_MeleeWeaponAttack::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (OwnerActor)
	{
		UE_LOG(LogTemp, Log, TEXT("Event Tag: %s for Actor: %s"), *InstanceData.EventTag.ToString(), *OwnerActor->GetName());
	}

	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus FStateTreeTask_MeleeWeaponAttack::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return FStateTreeTaskCommonBase::Tick(Context, DeltaTime);
}

void FStateTreeTask_MeleeWeaponAttack::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}

EStateTreeRunStatus FStateTreeTask_MeleeWeaponAttack::ActivateMeleeAttack(const FStateTreeExecutionContext& Context) const
{
	return EStateTreeRunStatus::Running;
}
