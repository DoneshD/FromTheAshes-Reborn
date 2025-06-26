#include "StateTree/MeleeTaskTest.h"

#include "Abilities/GameplayAbilityTypes.h"


EStateTreeRunStatus FMeleeTaskTestStruct::EnterState(FStateTreeExecutionContext& Context,
                                                     const FStateTreeTransitionResult& Transition) const
{
	return FStateTreeTaskBase::EnterState(Context, Transition);
}

EStateTreeRunStatus FMeleeTaskTestStruct::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return FStateTreeTaskBase::Tick(Context, DeltaTime);
}

void FMeleeTaskTestStruct::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskBase::ExitState(Context, Transition);
}

void FMeleeTaskTestStruct::OnGameplayEventReceived(FGameplayEventData Payload)
{
	
}
