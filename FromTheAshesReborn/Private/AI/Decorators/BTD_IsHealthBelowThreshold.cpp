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
	APawn* TargetActor = OwnerComp.GetAIOwner()->GetPawn();

	if (TargetActor)
	{
		AEnemyBase* EnemyBase = Cast<AEnemyBase>(TargetActor);
		if (EnemyBase)
		{
			float MaxHealth = EnemyBase->NativeGetMaxHealth();
			float CurrentHealth = EnemyBase->NativeGetCurrentHealth();

			return CurrentHealth / MaxHealth <= HealthThreshold;
		}
	}
	return false;
}
