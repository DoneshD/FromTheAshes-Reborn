#include "AI/Tasks/BTTask_LightMeleeAttack.h"
#include "AIController.h"
#include "Characters/EnemyMelee.h"

UBTTask_LightMeleeAttack::UBTTask_LightMeleeAttack()
{
	NodeName = TEXT("BTTask_LightMeleeAttack");
}

EBTNodeResult::Type UBTTask_LightMeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	EnemyOwnerComp = &OwnerComp;

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyMelee* EnemyMelee = Cast<AEnemyMelee>(Pawn);

	if (!EnemyMelee)
	{
		return EBTNodeResult::Failed;
	}

	EnemyMelee->FindComponentByClass<UAttacksComponent>()->
		OnAttackEnd.BindUObject(this, &UBTTask_LightMeleeAttack::FinishedAttacking);

	EnemyMelee->LightAttack();
	return EBTNodeResult::InProgress;
}

void UBTTask_LightMeleeAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UBTTask_LightMeleeAttack::FinishedAttacking()
{
	if (EnemyOwnerComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("FinishedAttacking"));
		OnTaskFinished(*EnemyOwnerComp, nullptr, EBTNodeResult::Succeeded);
	}
}