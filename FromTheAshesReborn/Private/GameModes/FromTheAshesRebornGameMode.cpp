
#include "GameModes/FromTheAshesRebornGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/EnemyMelee.h"
#include "AIController.h"
#include "EventManagers/EnemySpawner.h"
#include "UObject/ConstructorHelpers.h"

AFromTheAshesRebornGameMode::AFromTheAshesRebornGameMode()
{
	
}

void AFromTheAshesRebornGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	OnEnemySpawned.BindUObject(this, &AFromTheAshesRebornGameMode::IncrementEnemyCount);
	OnEnemyDeath.BindUObject(this, &AFromTheAshesRebornGameMode::CheckEnemyCount);

}

void AFromTheAshesRebornGameMode::PublishEnemySpawned()
{
	OnEnemySpawned.Execute();
}

void AFromTheAshesRebornGameMode::PublishEnemyDeath()
{
	OnEnemyDeath.Execute();
}

void AFromTheAshesRebornGameMode::SpawnMeleeEnemy()
{
	AActor* EnemySpawnerInstance = UGameplayStatics::GetActorOfClass(GetWorld(), EnemySpawnerClass);
	EnemySpawner = Cast<AEnemySpawner>(EnemySpawnerInstance);

	SpawnerLocation = EnemySpawner->GetActorLocation();
	AAIControllerEnemyBase* AIController = GetWorld()->SpawnActor<AAIControllerEnemyBase>(SpawnParams);
	//EnemyMelee = GetWorld()->SpawnActor<AEnemyMelee>(EnemyMeleeClass, SpawnerLocation, SpawnRotation, SpawnParams);
	EnemyMelee = GetWorld()->SpawnActorDeferred<AEnemyMelee>(EnemyMeleeClass, FTransform(
		SpawnRotation, SpawnerLocation), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	if (EnemyMelee)
	{
		// Assuming you have the AIController class ready

		if (AIController)
		{
			AIController->Possess(EnemyMelee);
			UGameplayStatics::FinishSpawningActor(EnemyMelee, FTransform(SpawnRotation, SpawnerLocation));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn AI Controller"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Enemy Melee"));
	}
	
}

void AFromTheAshesRebornGameMode::IncrementEnemyCount()
{
	EnemiesCount += 1;
}

void AFromTheAshesRebornGameMode::CheckEnemyCount()
{
	EnemiesCount -= 1;

	if (EnemiesCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("RIGHT All Enemies Eliminated"));
	}
}
