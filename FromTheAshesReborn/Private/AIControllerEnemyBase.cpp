// Fill out your copyright notice in the Description page of Project Settings.

#include "AIControllerEnemyBase.h"
#include "Enums/EAIStates.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Characters/EnemyBase.h"

AAIControllerEnemyBase::AAIControllerEnemyBase()
{
	//BaseBlkackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComponent");

	AISenseConfigSight = CreateDefaultSubobject<UAISenseConfig_Sight>("SenseSight");
	AISenseConfigSight->SightRadius = 800.0f;
	AISenseConfigSight->LoseSightRadius = 1200.0f;
	AISenseConfigSight->PeripheralVisionAngleDegrees = 60.0f;
	AISenseConfigSight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfigSight->DetectionByAffiliation.bDetectFriendlies = true;
	AISenseConfigSight->DetectionByAffiliation.bDetectNeutrals = true;
	AISenseConfigSight->SetMaxAge(5.0f);

	AISenseConfigHearing = CreateDefaultSubobject<UAISenseConfig_Hearing>("SenseHearing");
	AISenseConfigHearing->HearingRange = 700.0f;
	AISenseConfigHearing->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfigHearing->DetectionByAffiliation.bDetectFriendlies = true;
	AISenseConfigHearing->DetectionByAffiliation.bDetectNeutrals = true;
	AISenseConfigHearing->SetMaxAge(3.0f);

	AISenseConfigDamage = CreateDefaultSubobject<UAISenseConfig_Damage>("SenseDamage");
	AISenseConfigDamage->SetMaxAge(5.0f);

	AIPerceptionComponent->ConfigureSense(*AISenseConfigSight);
	AIPerceptionComponent->ConfigureSense(*AISenseConfigHearing);
	AIPerceptionComponent->ConfigureSense(*AISenseConfigDamage);
	AIPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());

	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIControllerEnemyBase::ActorsPerceptionUpdated);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIControllerEnemyBase::TargetActorsPerceptionUpdated);
}

void AAIControllerEnemyBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn);
	if (Enemy)
	{
		if (Enemy->BaseBehaviorTree)
		{
			RunBehaviorTree(Enemy->BaseBehaviorTree);
			SetStateAsPassive();
			Enemy->NativeGetIdealRange(AttackRadius, DefendRadius);
			UE_LOG(LogTemp, Warning, TEXT("OnPossess() - Ideal Attack Radius: %f, Ideal Defend Radius: %f"), AttackRadius, DefendRadius);
			GetBlackboardComponent()->SetValueAsFloat(AttackRadiusKeyName, AttackRadius);
			GetBlackboardComponent()->SetValueAsFloat(DefendRadiusKeyName, DefendRadius);

		}
	}
}

void AAIControllerEnemyBase::BeginPlay()
{
	Super::BeginPlay();

}

void AAIControllerEnemyBase::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{

	for (AActor* Actor : UpdatedActors)
	{
		if (CanSenseActor(Actor, EAISenses::EAISenses_Sight, OutSightInfo))
		{
			HandleSensedSight(Actor);
		}
		if (CanSenseActor(Actor, EAISenses::EAISenses_Hearing, OutHearInfo))
		{
			HandleSensedSound(OutHearInfo.StimulusLocation);
		}
		if (CanSenseActor(Actor, EAISenses::EAISenses_Damage, OutDamageInfo))
		{
			HandleSensedDamage(Actor);
		}
	}
}

void AAIControllerEnemyBase::TargetActorsPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{

}

EAIStates AAIControllerEnemyBase::GetCurrentState()
{
	//fix
	return EAIStates::EAIStates_Passive;
	GetBlackboardComponent()->GetValueAsEnum(StateKeyName);
}

void AAIControllerEnemyBase::SetStateAsPassive()
{
	GetBlackboardComponent()->SetValueAsEnum(StateKeyName, static_cast<uint8>(EAIStates::EAIStates_Passive));
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
		GetBlackboardComponent()->SetValueAsObject(AttackTargetKeyName, NewAttackTarget);
		GetBlackboardComponent()->SetValueAsEnum(StateKeyName, static_cast<uint8>(EAIStates::EAIStates_Attacking));
		AttackTarget = NewAttackTarget;
		// Assuming AttackTargetKeyName and StateKeyName are FName variables
		UE_LOG(LogTemp, Warning, TEXT("AttackTargetKeyName: %s, StateKeyName: %s"),
			*AttackTargetKeyName.ToString(), *StateKeyName.ToString());


	}
	else
	{
		SetStateAsPassive();
	}

}

void AAIControllerEnemyBase::SetStateAsInvestigating(FVector Location)
{
	GetBlackboardComponent()->SetValueAsEnum(StateKeyName, static_cast<uint8>(EAIStates::EAIStates_Investigating));
	GetBlackboardComponent()->SetValueAsVector(PointOfInterestKeyName, Location);
}

void AAIControllerEnemyBase::SetStateAsFrozen()
{
	GetBlackboardComponent()->SetValueAsEnum(StateKeyName, static_cast<uint8>(EAIStates::EAIStates_Frozen));
}

void AAIControllerEnemyBase::SetStateAsDead()
{
	GetBlackboardComponent()->SetValueAsEnum(StateKeyName, static_cast<uint8>(EAIStates::EAIStates_Dead));
}

bool AAIControllerEnemyBase::CanSenseActor(AActor* Actor, EAISenses Sense, FAIStimulus& CurrentStimulus)
{
	FActorPerceptionBlueprintInfo Info;
	bool bSightSensed = AIPerceptionComponent->GetActorsPerception(Actor, Info);

	//might be a problem
	for (const FAIStimulus& CurrentStimulus : Info.LastSensedStimuli)
	{
		FAISenseID SightID = AISenseConfigSight->GetSenseID();
		FAISenseID HearID = AISenseConfigHearing->GetSenseID();
		FAISenseID DamageId = AISenseConfigDamage->GetSenseID();
		
		if (Sense == EAISenses::EAISenses_Sight)
		{
			if (CurrentStimulus.Type == SightID)
			{
				return CurrentStimulus.WasSuccessfullySensed();
			}
		}
		else if (Sense == EAISenses::EAISenses_Hearing)
		{
			if (CurrentStimulus.Type == HearID)
			{
				return CurrentStimulus.WasSuccessfullySensed();

			}
		}
		else if (Sense == EAISenses::EAISenses_Damage)
		{
			if (CurrentStimulus.Type == DamageId)
			{
				return CurrentStimulus.WasSuccessfullySensed();
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Sense not found"));
			return false;
		}
	}
	return false;
}


void AAIControllerEnemyBase::HandleSensedSight(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleSensedSight()"));
	switch (GetCurrentState())
	{
	case EAIStates::EAIStates_Passive:
		SetStateAsAttacking(Actor, false);


	case EAIStates::EAIStates_Attacking:
		//Nothing

	case EAIStates::EAIStates_Investigating:
		SetStateAsAttacking(Actor, false);

	case EAIStates::EAIStates_Frozen:
		//Nothing

	case EAIStates::EAIStates_Dead:
		//Nothing

	default:
		break;
	}
}

void AAIControllerEnemyBase::HandleSensedSound(FVector Location)
{
	switch (GetCurrentState())
	{
	case EAIStates::EAIStates_Passive:
		SetStateAsInvestigating(Location);

	case EAIStates::EAIStates_Attacking:
		//Nothing

	case EAIStates::EAIStates_Investigating:
		SetStateAsInvestigating(Location);

	case EAIStates::EAIStates_Frozen:
		//Nothing

	case EAIStates::EAIStates_Dead:
		//Nothing

	default:
		break;
	}
}

void AAIControllerEnemyBase::HandleSensedDamage(AActor* Actor)
{
	switch (GetCurrentState())
	{
	case EAIStates::EAIStates_Passive:
		SetStateAsAttacking(Actor, false);

	case EAIStates::EAIStates_Attacking:
		//Nothing

	case EAIStates::EAIStates_Investigating:
		SetStateAsAttacking(Actor, false);

	case EAIStates::EAIStates_Frozen:
		//Nothing

	case EAIStates::EAIStates_Dead:
		//Nothing

	default:
		break;
	}
}
