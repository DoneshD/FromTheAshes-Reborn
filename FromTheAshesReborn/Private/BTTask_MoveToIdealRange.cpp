// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToIdealRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Characters/EnemyBase.h"


UBTTask_MoveToIdealRange::UBTTask_MoveToIdealRange()
{
	NodeName = TEXT("BTTask_MoveToIdealRange");

}

EBTNodeResult::Type UBTTask_MoveToIdealRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(TargetActor);
	MoveRequest.SetAcceptanceRadius(OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName));
	//Might need a pawn
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyBase* Enemy = Cast<AEnemyBase>(Pawn);

	OwnerComp.GetAIOwner()->MoveTo(MoveRequest);
	return EBTNodeResult::Succeeded;
}
