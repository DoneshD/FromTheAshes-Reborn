#include "Ai/Controllers/AIControllerEnemyBase.h"
#include "Enums/EAIStates.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Navigation/CrowdFollowingComponent.h"
//cheeky for now
#include "Interfaces/DashingCombatantInterface.h"
#include "Characters/EnemyBase.h"

AAIControllerEnemyBase::AAIControllerEnemyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
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


}

void AAIControllerEnemyBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);


	AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn);
	if (Enemy)
	{
		if (Enemy->BehaviorTree)
		{
			RunBehaviorTree(Enemy->BehaviorTree);
			SetStateAsPassive();
			Enemy->GetIdealRange(AttackRadius, DefendRadius, OrbitRadius);
			GetBlackboardComponent()->SetValueAsFloat(AttackRadiusKeyName, AttackRadius);
			GetBlackboardComponent()->SetValueAsFloat(DefendRadiusKeyName, DefendRadius);
			GetBlackboardComponent()->SetValueAsFloat(OrbitRadiusKeyName, OrbitRadius);
		}
	}
}

void AAIControllerEnemyBase::BeginPlay()
{
	Super::BeginPlay();

}

void AAIControllerEnemyBase::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{

	/*
	for (AActor* Actor : UpdatedActors)
	{
		OutSightStimuliInfo = CanSenseActor(Actor, EAISenses::EAISenses_Sight);
		if (OutSightStimuliInfo.WasSuccessfullySensed())
		{
			//temporary
			IDashingCombatantInterface* PlayableCharacter = Cast<IDashingCombatantInterface>(Actor);
			if (PlayableCharacter)
			{
				HandleSensedSight(Actor);
			}
		}

		OutHearStimuliInfo = CanSenseActor(Actor, EAISenses::EAISenses_Hearing);
		if (OutHearStimuliInfo.WasSuccessfullySensed())
		{
			HandleSensedSound(OutHearStimuliInfo.StimulusLocation);
		}

		OutDamageStimuliInfo = CanSenseActor(Actor, EAISenses::EAISenses_Damage);
		if (OutDamageStimuliInfo.WasSuccessfullySensed())
		{
			HandleSensedDamage(Actor);
		}
	}
	*/
}

EAIStates AAIControllerEnemyBase::GetCurrentState()
{
	EAIStates CurrentState = static_cast<EAIStates>(GetBlackboardComponent()->GetValueAsEnum(StateKeyName));
	return CurrentState;
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
	}
	else
	{
		SetStateAsPassive();
	}
	
	
}

void AAIControllerEnemyBase::SetStateAsOrbiting(AActor* IncomingAttackTarget)
{
	GetBlackboardComponent()->SetValueAsEnum(StateKeyName, static_cast<uint8>(EAIStates::EAIStates_Orbiting));

	if (IncomingAttackTarget)
	{
		GetBlackboardComponent()->SetValueAsObject(AttackTargetKeyName, IncomingAttackTarget);
		GetBlackboardComponent()->SetValueAsEnum(StateKeyName, static_cast<uint8>(EAIStates::EAIStates_Orbiting));
		AttackTarget = IncomingAttackTarget;
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

const FAIStimulus AAIControllerEnemyBase::CanSenseActor(AActor* Actor, EAISenses Sense)
{
	FActorPerceptionBlueprintInfo Info;
	bool bSightSensed = AIPerceptionComponent->GetActorsPerception(Actor, Info);

	for (const FAIStimulus& CurrentStimulus : Info.LastSensedStimuli)
	{
		FAISenseID SightID = AISenseConfigSight->GetSenseID();
		FAISenseID HearID = AISenseConfigHearing->GetSenseID();
		FAISenseID DamageId = AISenseConfigDamage->GetSenseID();
		
		if (Sense == EAISenses::EAISenses_Sight)
		{
			if (CurrentStimulus.Type == SightID)
			{
				return CurrentStimulus;
			}
		}
		else if (Sense == EAISenses::EAISenses_Hearing)
		{
			if (CurrentStimulus.Type == HearID)
			{
				return CurrentStimulus;

			}
		}
		else if (Sense == EAISenses::EAISenses_Damage)
		{
			if (CurrentStimulus.Type == DamageId)
			{
				return CurrentStimulus;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Sense not found"));
			return CurrentStimulus;
		}
	
	}
	const FAIStimulus DummyStimuli;
	return DummyStimuli;
}


void AAIControllerEnemyBase::HandleSensedSight(AActor* Actor)
{
	switch (GetCurrentState())
	{
	case EAIStates::EAIStates_Passive:
		SetStateAsAttacking(Actor, true);

	case EAIStates::EAIStates_Attacking:
		//Nothing

	case EAIStates::EAIStates_Investigating:
		SetStateAsAttacking(Actor, true);

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
		SetStateAsAttacking(Actor, true);

	case EAIStates::EAIStates_Attacking:
		//Nothing

	case EAIStates::EAIStates_Investigating:
		SetStateAsAttacking(Actor, true);

	case EAIStates::EAIStates_Frozen:
		//Nothing

	case EAIStates::EAIStates_Dead:
		//Nothing

	default:
		break;
	}
}

