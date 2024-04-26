#include "CombatManager.h"
#include "AttackTargetInterface.h"
#include "AttackerInterface.h"

ACombatManager::ACombatManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
	IAttackTargetInterface* AttackTargetInterface = Cast<IAttackTargetInterface>(AttackTarget);
	MaxAttackersCount = AttackTargetInterface->GetMaxAttackersCount();
	
}

void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACombatManager::HandleAttackRequest(TObjectPtr<AActor> Attacker)
{
	if (Attacker != AttackTarget)
	{
		if (CurrentAttackers.Num() < MaxAttackersCount)
		{
			CurrentAttackers.AddUnique(Attacker);
			IAttackerInterface* AttackerInterface = Cast<IAttackerInterface>(Attacker);
			AttackerInterface->Attack(AttackTarget);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentAttackers.Num() > MaxAttackersCount, Adding to WaitingArray"));

			WaitingAttackers.AddUnique(Attacker);
			IAttackerInterface* AttackerInterface = Cast<IAttackerInterface>(Attacker);
			AttackerInterface->Wait(AttackTarget);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR: Attacker == AttackTarget"));
	}
}

