#include "AI/Tasks/BTTask_LightMeleeAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Characters/EnemyBase.h"

UBTTask_LightMeleeAttack::UBTTask_LightMeleeAttack()
{
	NodeName = TEXT("BTTask_LightMeleeAttack");
}

EBTNodeResult::Type UBTTask_LightMeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	EnemyOwnerComp = &OwnerComp;

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyBase* EnemyBase = Cast<AEnemyBase>(Pawn);

	if (!EnemyBase)
	{
		return EBTNodeResult::Failed;
	}

	EnemyBase->OnAttackEnd.BindUObject(this, &UBTTask_LightMeleeAttack::FinishedAttacking);

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	//if (EnemyBase->AttackStart(TargetActor, 1))
	
	EnemyBase->LightAttack();
	
	return EBTNodeResult::Succeeded;
}

void UBTTask_LightMeleeAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyBase* EnemyBase = Cast<AEnemyBase>(Pawn);

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(TargetActor);

	UE_LOG(LogTemp, Warning, TEXT("OnTaskFinished"));
	IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(EnemyBase);
	if (AIEnemyInterface)
	{
		UE_LOG(LogTemp, Warning, TEXT("Calling AttackEnd"));
		AIEnemyInterface->AttackEnd(TargetActor);
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UBTTask_LightMeleeAttack::FinishedAttacking()
{
	if (EnemyOwnerComp)
	{
		OnTaskFinished(*EnemyOwnerComp, nullptr, EBTNodeResult::Succeeded);
	}
}