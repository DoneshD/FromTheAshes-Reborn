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
	EnemyMelee->FindComponentByClass<UAttacksComponent>()->
		OnAttackEnd.AddDynamic(this, &UBTTask_LightMeleeAttack::FinishedAttacking);
	EnemyMelee->LightAttack();
	return EBTNodeResult::InProgress;
}

void UBTTask_LightMeleeAttack::FinishedAttacking()
{
	UE_LOG(LogTemp, Warning, TEXT("bDoneAttacking = true"));
	//return EBTNodeResult::Succeeded;

}
