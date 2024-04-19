
#include "CombatManager.h"
#include "AttackerInterface.h"
#include "AttackTargetInterface.h"

ACombatManager::ACombatManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
	IAttackTargetInterface* AttackTargetInterface = Cast<IAttackTargetInterface>(AttackTarget);
	if (AttackTargetInterface)
	{
		MaxAttackersCount = AttackTargetInterface->GetMaxAttackersCount();
	}
	
}

void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACombatManager::HandleAttackRequest(TObjectPtr<AActor> Attacker)
{
	if (Attacker != AttackTarget)
	{
		if (Attackers.Num() < MaxAttackersCount)
		{
			Attackers.AddUnique(Attacker);
			IAttackerInterface* AttackerInterface = Cast<IAttackerInterface>(Attacker);
			if (AttackerInterface)
			{
				UE_LOG(LogTemp, Warning, TEXT("AttackerInterface->Attack(AttackTarget);"));
				AttackerInterface->Attack(AttackTarget);
			}
		}
		else
		{
			WaitingAttackers.AddUnique(Attacker);
			IAttackerInterface* AttackerInterface = Cast<IAttackerInterface>(Attacker);
			if (AttackerInterface)
			{
				UE_LOG(LogTemp, Warning, TEXT("AttackerInterface->Wait(AttackTarget)"));
				AttackerInterface->Wait(AttackTarget);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CAN NOT Attacker == AttackTarget"));
	}
}

