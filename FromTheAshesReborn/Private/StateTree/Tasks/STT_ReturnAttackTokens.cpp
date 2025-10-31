#include "StateTree/Tasks/STT_ReturnAttackTokens.h"
#include "StateTreeExecutionContext.h"
#include "CombatComponents/GroupCombatComponent.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "Enemy/GroupCombatSubsystem.h"
#include "FTACustomBase/FTACharacter.h"

EStateTreeRunStatus FStateTreeTask_ReturnAttackTokens::EnterState(FStateTreeExecutionContext& Context,const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if(!InstanceData.TargetActor && !InstanceData.TargetActor->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("Target is null"))
		return EStateTreeRunStatus::Failed;
	}
	
	AFTACharacter* FTACharTarget = Cast<AFTACharacter>(InstanceData.TargetActor);

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

	AEnemyBaseCharacter* Enemy = Cast<AEnemyBaseCharacter>(InstanceData.OwnerActor);

	UGroupCombatSubsystem* GCS = Context.GetWorld()->GetSubsystem<UGroupCombatSubsystem>();
	if(GCS)
	{
		if(GCS->EnemiesAttackTokensMap.Find(Enemy))
		{
			TargetGCC->AttackTokensCount += GCS->EnemiesAttackTokensMap[Enemy];
			GCS->EnemiesAttackTokensMap.Remove(Enemy);
			
			return EStateTreeRunStatus::Succeeded;
		}
		UE_LOG(LogTemp, Error, TEXT("Owner not found"));
		return EStateTreeRunStatus::Failed;
	}
	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FStateTreeTask_ReturnAttackTokens::Tick(FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	return FStateTreeTaskCommonBase::Tick(Context, DeltaTime);
}

void FStateTreeTask_ReturnAttackTokens::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}
