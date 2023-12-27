// Fill out your copyright notice in the Description page of Project Settings.

#include "AIControllerEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EnemyBase.h"

void AAIControllerEnemyBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn);
	if (Enemy)
	{
		if (BaseBehaviorTree)
		{
			RunBehaviorTree(BaseBehaviorTree);
			SetStateAsPassive();
		}
		
	}
}

EAIStates AAIControllerEnemyBase::GetCurrentState()
{
	return EAIStates();
}

void AAIControllerEnemyBase::SetStateAsPassive()
{
	BaseBlackboardComponent->SetValueAsEnum(StateKeyName, static_cast<uint8>(EAIStates::EAIStates_Passive));
}

void AAIControllerEnemyBase::SetStateAsAttacking(AActor* AttackTarget, bool UseLastKnownAttackTarget)
{

}

void AAIControllerEnemyBase::SetStateAsInvestigating()
{

}

void AAIControllerEnemyBase::SetStateAsInvestigating(FVector Location)
{

}

void AAIControllerEnemyBase::SetStateAsFrozen()
{

}

void AAIControllerEnemyBase::SetStateAsDead()
{

}
