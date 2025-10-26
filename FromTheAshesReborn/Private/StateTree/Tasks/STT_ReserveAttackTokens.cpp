#include "StateTree/Tasks/STT_ReserveAttackTokens.h"

#include "StateTreeExecutionContext.h"
#include "CombatComponents/GroupCombatComponent.h"
#include "FTACustomBase/FTACharacter.h"

EStateTreeRunStatus FStateTreeTask_ReserveAttackTokens::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if(!InstanceData.Target && !InstanceData.Target->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("Target is null"))
		return EStateTreeRunStatus::Failed;

	}
	
	AFTACharacter* FTACharTarget = Cast<AFTACharacter>(InstanceData.Target);

	if(!FTACharTarget && !FTACharTarget->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("FTACharTarget is null"))
		return EStateTreeRunStatus::Failed;
	}

	UGroupCombatComponent* TargetGCC = FTACharTarget->FindComponentByClass<UGroupCombatComponent>();

	if(!TargetGCC && !TargetGCC->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("TargetGCC is null"))
		return EStateTreeRunStatus::Failed;
	}

	TargetGCC->AttackTokensCount -= InstanceData.TokenAmount;

	// UE_LOG(LogTemp, Warning, TEXT("Test Reserve"));

	return EStateTreeRunStatus::Succeeded;
	
}

EStateTreeRunStatus FStateTreeTask_ReserveAttackTokens::Tick(FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	return FStateTreeTaskCommonBase::Tick(Context, DeltaTime);
}

void FStateTreeTask_ReserveAttackTokens::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}
