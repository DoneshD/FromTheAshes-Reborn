// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/BTD_IsHealthBelowThreshold.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/Controllers/AIControllerEnemyBase.h"
#include "Characters/EnemyBase.h"

UBTD_IsHealthBelowThreshold::UBTD_IsHealthBelowThreshold()
{
}

bool UBTD_IsHealthBelowThreshold::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (OwnerPawn)
	{
		AEnemyBase* EnemyBase = Cast<AEnemyBase>(OwnerPawn);
		if (EnemyBase)
		{
			float MaxHealth = EnemyBase->GetMaxHealth();
			float CurrentHealth = EnemyBase->GetCurrentHealth();

			return CurrentHealth / MaxHealth <= HealthThreshold;
		}
	}
	return false;
}
