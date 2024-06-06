#include "EventManagers/CombatManager.h"
#include "Interfaces/AttackTargetInterface.h"
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

void ACombatManager::HandleAttackRequest(AActor* Attacker)
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
			UE_LOG(LogTemp, Warning, TEXT("AIEnemyInterface->SetStateAsAttacking"));
			AIEnemyInterface->SetStateAsAttacking(AttackTarget);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentAttackers.Num() > MaxAttackersCount, Adding to Orbiters"));

			Oribters.AddUnique(Attacker);
			IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Attacker);
			UE_LOG(LogTemp, Warning, TEXT("AIEnemyInterface->SetStateAsOrbiting"));
			AIEnemyInterface->SetStateAsOrbiting(AttackTarget);

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR: Attacker == AttackTarget"));
	}
}

void ACombatManager::HandleAttackerSwapRequest(AActor* Attacker)
{
	if (!Attacker)
	{
		return;
	}

	if (Attacker != AttackTarget)
	{
		IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Attacker);
		if (AIEnemyInterface->GetCurrentState() == EAIStates::EAIStates_Attacking)
		{
			UE_LOG(LogTemp, Warning, TEXT("NICE!!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("OK!!"));

		}

	}
}

void ACombatManager::EngageOrbiter()
{
	if (Oribters.Num() > 0)
	{
		int RemoveIndex = FMath::RandRange(0, Oribters.Num() - 1);

		if (Oribters.IsValidIndex(RemoveIndex))
		{
			AActor* Oribiter = Oribters[RemoveIndex];

			Oribters.RemoveAt(RemoveIndex);

			if (Oribiter)
			{
				Attackers.AddUnique(Oribiter);
				IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Oribiter);
				if (AIEnemyInterface)
				{
					AIEnemyInterface->SetStateAsAttacking(AttackTarget);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Oribiter does not implement IAIEnemyInterface"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Oribiter is null"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid index"));
		}
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

void ACombatManager::ToggleAggro()
{

}

