#include "AI/Tasks/BTT_AttackStart.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EnemyBase.h"

UBTT_AttackStart::UBTT_AttackStart()
{
}

EBTNodeResult::Type UBTT_AttackStart::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyBase* Enemy = Cast<AEnemyBase>(Pawn);

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	UE_LOG(LogTemp, Warning, TEXT("TokensNeeded: %f"), TokensNeeded);

	if (Enemy->AttackStart(TargetActor, TokensNeeded))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack Start Succedeed"));
		return EBTNodeResult::Succeeded;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack Start Failed"));
		return EBTNodeResult::Failed;
	}
	UE_LOG(LogTemp, Warning, TEXT("Attack Start None"));
}
