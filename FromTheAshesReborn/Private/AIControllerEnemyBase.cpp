// Fill out your copyright notice in the Description page of Project Settings.

#include "AIControllerEnemyBase.h"
#include "Characters/EnemyBase.h"

void AAIControllerEnemyBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Warning, TEXT("Test"));
	AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn);
	if (Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnPossess: %s"), *Enemy->GetName());
		if (AIBehaviorTree)
		{
			UE_LOG(LogTemp, Warning, TEXT("YES"));

			RunBehaviorTree(AIBehaviorTree);
		}
		
	}
}
