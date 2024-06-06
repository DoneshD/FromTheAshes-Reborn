#include "EventManagers/CombatManager.h"
#include "Interfaces/AttackTargetInterface.h"
#include "Interfaces/AIEnemyInterface.h"

ACombatManager::ACombatManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ACombatManager::BeginPlay()
{
	Super::BeginPlay();

	AttackTarget = GetWorld()->GetFirstPlayerController()->GetPawn();

	IAttackTargetInterface* AttackTargetInterface = Cast<IAttackTargetInterface>(GetWorld()->GetFirstPlayerController()->GetPawn());
	MaxAttackersCount = AttackTargetInterface->GetMaxAttackersCount();
}

void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACombatManager::HandleAttackRequest(AActor* Attacker)
{
	if (!Attacker)
	{
		return;
	}
	
	if (Attacker != AttackTarget)
	{
		IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Attacker);
		if (!AIEnemyInterface)
		{
			return;
		}

		if (Attackers.Num() < MaxAttackersCount)
		{
			Attackers.AddUnique(Attacker);
			AIEnemyInterface->SetStateAsAttacking(AttackTarget);
		}
		else
		{
			Oribters.AddUnique(Attacker);
			AIEnemyInterface->SetStateAsOrbiting(AttackTarget);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR: Attacker == AttackTarget"));
	}
}

void ACombatManager::HandleAttackerSwapRequest(AActor* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	if (Enemy != AttackTarget)
	{
		IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Enemy);
		if (AIEnemyInterface->GetCurrentState() == EAIStates::EAIStates_Orbiting)
		{
			Oribters.Remove(Enemy);
			DisengageRandomAttacker();
			Attackers.AddUnique(Enemy);

			AIEnemyInterface->SetStateAsAttacking(AttackTarget);
		}
		else if (AIEnemyInterface->GetCurrentState() == EAIStates::EAIStates_Attacking)
		{
			Attackers.Remove(Enemy);
			EngageRandomOrbiter();
			Oribters.AddUnique(Enemy);

			AIEnemyInterface->SetStateAsOrbiting(AttackTarget);
		}
	}
}

void ACombatManager::EngageRandomOrbiter()
{
	if (Oribters.Num() > 0)
	{
		int RemoveIndex = FMath::RandRange(0, Oribters.Num() - 1);

		if (!Oribters.IsValidIndex(RemoveIndex))
		{
			return;
		}

		AActor* Oribiter = Oribters[RemoveIndex];

		if (!Oribiter)
		{
			return;
		}

		Oribters.RemoveAt(RemoveIndex);
		Attackers.AddUnique(Oribiter);
		
		IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Oribiter);
		if (!AIEnemyInterface)
		{
			return;
		}

		AIEnemyInterface->SetStateAsAttacking(AttackTarget);
	}
}

void ACombatManager::DisengageRandomAttacker()
{
	if (Attackers.Num() > 0)
	{
		int RemoveIndex = FMath::RandRange(0, Attackers.Num() - 1);

		if (!Attackers.IsValidIndex(RemoveIndex))
		{
			return;
		}
		AActor* Attacker = Attackers[RemoveIndex];

		if (!Attacker)
		{
			return;
		}
		Attackers.RemoveAt(RemoveIndex);

		if (!Attacker)
		{
			return;
		}

		Oribters.AddUnique(Attacker);

		IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Attacker);
		if (!AIEnemyInterface)
		{
			return;
		}

		AIEnemyInterface->SetStateAsOrbiting(AttackTarget);
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
			if (AIEnemyInterface)
			{
				AIEnemyInterface->Retreat();
			}
		}

		Attackers.Empty();
		Oribters.Empty();
	}
	
	else
	{
		if (Attackers.Contains(ActorRef))
		{
			Attackers.Remove(ActorRef);
			EngageRandomOrbiter();
		}
		else
		{
			if (Oribters.Contains(ActorRef))
			{
				Oribters.Remove(ActorRef);
				EngageRandomOrbiter();
			}
		}
	}
}
