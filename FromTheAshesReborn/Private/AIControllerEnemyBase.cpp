// Fill out your copyright notice in the Description page of Project Settings.

#include "AIControllerEnemyBase.h"
#include "Enums/EAIStates.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Characters/EnemyBase.h"

AAIControllerEnemyBase::AAIControllerEnemyBase()
{
	BaseBlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComponent");
	AISenseConfigSight = CreateDefaultSubobject<UAISenseConfig_Sight>("SenseSight");
	AISenseConfigSight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfigSight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfigSight->DetectionByAffiliation.bDetectNeutrals = false;

	AISenseConfigHearing = CreateDefaultSubobject<UAISenseConfig_Hearing>("SenseHearing");
	AISenseConfigHearing->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfigHearing->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfigHearing->DetectionByAffiliation.bDetectNeutrals = false;

	AIPerceptionComponent->ConfigureSense(*AISenseConfigSight);
	AIPerceptionComponent->ConfigureSense(*AISenseConfigHearing);
	AIPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
}

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
	//fix
	return EAIStates::EAIStates_Passive;
	BaseBlackboardComponent->GetValueAsEnum(StateKeyName);
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
