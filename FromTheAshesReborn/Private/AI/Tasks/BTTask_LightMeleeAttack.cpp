// Fill out your copyright notice in the Description page of Project Settings.


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

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyMelee* EnemyMelee = Cast<AEnemyMelee>(Pawn);

	if (!EnemyMelee)
	{
		return EBTNodeResult::Failed;
	}

	EnemyMelee->LightAttack();

	//EnemyMelee->FindComponentByClass<UAttacksComponent>()->
		//OnAttackEnd.AddDynamic(this, &UBTTask_LightMeleeAttack::FinsihedAttacking);

	if (bDoneAttacking)
	{
		UE_LOG(LogTemp, Warning, TEXT("EBTNodeResult::Succeeded"));
		return EBTNodeResult::Succeeded;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EBTNodeResult::Failed"));
		return EBTNodeResult::Failed;
	}
}

void UBTTask_LightMeleeAttack::FinsihedAttacking()
{
	UE_LOG(LogTemp, Warning, TEXT("bDoneAttacking = true"));
	bDoneAttacking = true;
}
