// Fill out your copyright notice in the Description page of Project Settings.

#include "AIControllerEnemyBase.h"
#include "Enums/EAIStates.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
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

void AAIControllerEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIControllerEnemyBase::OnPerceptionUpdated);
}

void AAIControllerEnemyBase::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	UE_LOG(LogTemp, Warning, TEXT("OnPerceptionUpdated() called"));
	for (AActor* Actor : UpdatedActors)
	{
		if (CanSenseActor(Actor, EAISenses::EAISenses_Sight))
		{
			//HandleSensedSight(Actor);
		}
		if (CanSenseActor(Actor, EAISenses::EAISenses_Hearing))
		{
			//HandleSensedSound(Actor);
		}
		if (CanSenseActor(Actor, EAISenses::EAISenses_Damage))
		{
			//HandleSensedDamage(Actor);
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

bool AAIControllerEnemyBase::CanSenseActor(AActor* Actor, EAISenses Sense)
{
	FActorPerceptionBlueprintInfo Info;
	bool bSightSensed = AIPerceptionComponent->GetActorsPerception(Actor, Info);

	for (const FAIStimulus& CurrentStimulus : Info.LastSensedStimuli)
	{
		FAISenseID sightid = AISenseConfigSight->GetSenseID();
		FAISenseID hearid = AISenseConfigHearing->GetSenseID();

		if (CurrentStimulus.Type == sightid)
		{
			UE_LOG(LogTemp, Warning, TEXT("I SAW YOU!"));
			return true;
		}
		else if (CurrentStimulus.Type == hearid)
		{
			UE_LOG(LogTemp, Warning, TEXT("I HEARD YOU!"));
			return true;
		}
	}
	return false;
}
