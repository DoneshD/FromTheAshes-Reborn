#include "CombatManager.h"
#include "AttackTargetInterface.h"
#include "Characters/PlayableCharacter.h"
#include "AttackerInterface.h"

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

void ACombatManager::HandleDeath(TObjectPtr<AActor> ActorRef)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleDeath!!!"));

	
	if (!ActorRef)
	{
		return;
	}

	if (ActorRef == AttackTarget)
	{
		CurrentAttackers.Append(WaitingAttackers);
		for (TObjectPtr<AActor>  Attacker : CurrentAttackers)
		{
			IAttackerInterface* AttackerInterface = Cast<IAttackerInterface>(Attacker);
			AttackerInterface->Retreat();
		}
		CurrentAttackers.Empty();
		WaitingAttackers.Empty();

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR: Attacker == AttackTarget"));
	}
	
}

