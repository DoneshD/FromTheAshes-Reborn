// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_SetMovementSpeed.h"
#include "Characters/EnemyBase.h"

UBTTask_SetMovementSpeed::UBTTask_SetMovementSpeed()
{
	NodeName = TEXT("BTTask_SetMovementSpeed");

}

EBTNodeResult::Type UBTTask_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyBase* Enemy = Cast<AEnemyBase>(Pawn);

	if (!Enemy)
	{
		return EBTNodeResult::Failed;
	}

	Enemy->NativeSetMovementSpeed(MovementSpeed);
	return EBTNodeResult::Succeeded;
}
