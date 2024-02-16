// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ClearFocus.h"
#include "Characters/EnemyBase.h"

#include "AIController.h"

UBTTask_ClearFocus::UBTTask_ClearFocus()
{
	NodeName = TEXT("BTT_ClearFocus");

}

EBTNodeResult::Type UBTTask_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	//OwnerComp.GetAIOwner()->ClearFocus(EAIFocusPriority::Gameplay);

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyBase* Enemy = Cast<AEnemyBase>(Pawn);

	if (!Enemy)
	{
		return EBTNodeResult::Failed;
	}
	AAIController* EnemyController = Enemy->GetController<AAIController>();
	EnemyController->ClearFocus(EAIFocusPriority::Gameplay);

	return EBTNodeResult::Succeeded;
}
