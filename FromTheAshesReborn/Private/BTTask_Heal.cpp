// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Heal.h"
#include "Characters/EnemyBase.h"

UBTTask_Heal::UBTTask_Heal()
{
	NodeName = TEXT("BTTask_Heal");
}

EBTNodeResult::Type UBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyBase* Enemy = Cast<AEnemyBase>(Pawn);

	if (!Enemy)
	{
		return EBTNodeResult::Failed;
	}

	Enemy->NativeHeal(Enemy->NativeGetMaxHealth() * HealPercentage);
	return EBTNodeResult::Succeeded;
}
