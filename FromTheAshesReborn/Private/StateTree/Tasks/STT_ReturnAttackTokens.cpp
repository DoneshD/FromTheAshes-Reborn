#include "StateTree/Tasks/STT_ReturnAttackTokens.h"
#include "StateTreeExecutionContext.h"
#include "CombatComponents/GroupCombatComponent.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "Enemy/GroupCombatSubsystem.h"
#include "FTACustomBase/FTACharacter.h"
#include "Player/PlayerCharacter.h"

EStateTreeRunStatus FStateTreeTask_ReturnAttackTokens::EnterState(FStateTreeExecutionContext& Context,const FStateTreeTransitionResult& Transition) const
{
	// UE_LOG(LogTemp, Warning, TEXT("Return token"))
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if(!InstanceData.TargetActor && !InstanceData.TargetActor->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("Target is null"))
		return EStateTreeRunStatus::Failed;
	}
	
	APlayerCharacter* FTACharTarget = Cast<APlayerCharacter>(InstanceData.TargetActor);

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
			const int32 TokensTransferred = GCS->EnemiesAttackTokensMap[Enemy];
			const int32 PreviousAttackTokens = TargetGCC->AttackTokensCount;

			TargetGCC->AttackTokensCount += TokensTransferred;
			GCS->EnemiesAttackTokensMap.Remove(Enemy);

			// UE_LOG(LogTemp, Warning,
			// 	TEXT("Attack Tokens Transferred | Enemy: %s | Transferred: %d | Target Tokens: %d -> %d"),
			// 	*Enemy->GetName(),
			// 	TokensTransferred,
			// 	PreviousAttackTokens,
			// 	TargetGCC->AttackTokensCount
			// );
			return EStateTreeRunStatus::Succeeded;
		}
	
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
