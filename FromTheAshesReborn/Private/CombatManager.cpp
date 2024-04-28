#include "CombatManager.h"
#include "AttackTargetInterface.h"
#include "Characters/PlayableCharacter.h"
#include "Interfaces/AIEnemyInterface.h"

ACombatManager::ACombatManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ACombatManager::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	APlayableCharacter* PlayerCharacter = Cast<APlayableCharacter>(PlayerController->GetPawn());

	AttackTarget = PlayerCharacter;

	IAttackTargetInterface* AttackTargetInterface = Cast<IAttackTargetInterface>(PlayerCharacter);
	MaxAttackersCount = AttackTargetInterface->GetMaxAttackersCount();
}

void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACombatManager::HandleAttackRequest(TObjectPtr<AActor> Attacker)
{
	if (!Attacker)
	{
		return;
	}
	
	if (Attacker != AttackTarget)
	{
		if (Attackers.Num() < MaxAttackersCount)
		{
			Attackers.AddUnique(Attacker);
			IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Attacker);
			AIEnemyInterface->Attack(AttackTarget);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentAttackers.Num() > MaxAttackersCount, Adding to Orbiters"));

			Oribters.AddUnique(Attacker);
			IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Attacker);
			AIEnemyInterface->Wait(AttackTarget);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR: Attacker == AttackTarget"));
	}
}

void ACombatManager::EngageOrbiter()
{

	
	AActor* Oribter = Oribters[0];
	if (Oribter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Keean Dimitri Smith"));

		Oribters.Remove(Oribter);
		Attackers.AddUnique(Oribter);
		IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Oribter);
		AIEnemyInterface->Attack(AttackTarget);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("else Sth"));

	}
	
}


void ACombatManager::HandleDeath(TObjectPtr<AActor> ActorRef)
{
	if (!ActorRef)
	{
		return;
	}

	if (ActorRef == AttackTarget)
	{
		Attackers.Append(Oribters);
		for (TObjectPtr<AActor>  Attacker : Attackers)
		{
			IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Attacker);
			AIEnemyInterface->Retreat();
		}
		Attackers.Empty();
		Oribters.Empty();
	}
	
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attacker != AttackTarget"));
		if (Attackers.Contains(ActorRef))
		{
			Attackers.Remove(ActorRef);
			EngageOrbiter();
		}
		else
		{
			if (Oribters.Contains(ActorRef))
			{
				Oribters.Remove(ActorRef);
				EngageOrbiter();
			}
		}
	}
}

