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
	UE_LOG(LogTemp, Warning, TEXT("IN HandleAttackRequest"));
	// Check if the Attacker is valid
	if (Attacker)
	{
		// Get the name of the attacker and print it
		FString AttackerName = Attacker->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Attacker's Name: %s"), *AttackerName);
	}
	else
	{
		// Attacker is not valid
		UE_LOG(LogTemp, Error, TEXT("Invalid Attacker!"));
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

