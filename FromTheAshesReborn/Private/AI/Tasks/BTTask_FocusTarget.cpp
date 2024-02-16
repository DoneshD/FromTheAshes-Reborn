// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_FocusTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EnemyBase.h"

#include "AIController.h"
#

UBTTask_FocusTarget::UBTTask_FocusTarget()
{
	NodeName = TEXT("BTT_FocusTarget");
}

EBTNodeResult::Type UBTTask_FocusTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	//OwnerComp.GetAIOwner()->SetFocus(TargetActor, EAIFocusPriority::Gameplay);

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyBase* Enemy = Cast<AEnemyBase>(Pawn);

	if (!Enemy)
	{
		return EBTNodeResult::Failed;
	}
	AAIController* EnemyController = Enemy->GetController<AAIController>();
	EnemyController->SetFocus(TargetActor);

	return EBTNodeResult::Succeeded;
}
