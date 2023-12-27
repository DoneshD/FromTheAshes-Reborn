// Fill out your copyright notice in the Description page of Project Settings.

#include "AIControllerEnemyBase.h"
#include "Enums/EAIStates.h"
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
	BaseBlackboardComponent->GetValueAsEnum(StateKeyName);
	//fix
	return EAIStates::EAIStates_Passive;
}

void AAIControllerEnemyBase::SetStateAsPassive()
{
	BaseBlackboardComponent->SetValueAsEnum(StateKeyName, static_cast<uint8>(EAIStates::EAIStates_Passive));
}

void AAIControllerEnemyBase::SetStateAsAttacking(AActor* IncomingAttackTarget, bool UseLastKnownAttackTarget)
{
	AActor* NewAttackTarget;
	if (AttackTarget && UseLastKnownAttackTarget)
	{
		NewAttackTarget = AttackTarget;
	}
	else
	{
		NewAttackTarget = IncomingAttackTarget;
	}

	if (NewAttackTarget)
	{
		BaseBlackboardComponent->SetValueAsObject(AttackTargetKeyName, NewAttackTarget);
		BaseBlackboardComponent->SetValueAsEnum(StateKeyName, static_cast<uint8>(EAIStates::EAIStates_Attacking));
		AttackTarget = NewAttackTarget;
	}
	else
	{
		SetStateAsPassive();
	}

}

void AAIControllerEnemyBase::SetStateAsInvestigating(FVector Location)
{
	BaseBlackboardComponent->SetValueAsEnum(StateKeyName, static_cast<uint8>(EAIStates::EAIStates_Investigating));
	BaseBlackboardComponent->SetValueAsVector(PointOfInterestKeyName, Location);
}

void AAIControllerEnemyBase::SetStateAsFrozen()
{
	BaseBlackboardComponent->SetValueAsEnum(StateKeyName, static_cast<uint8>(EAIStates::EAIStates_Frozen));
}

void AAIControllerEnemyBase::SetStateAsDead()
{
	BaseBlackboardComponent->SetValueAsEnum(StateKeyName, static_cast<uint8>(EAIStates::EAIStates_Dead));
}
