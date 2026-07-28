#include "Level/WaveManager.h"

#include "Enemy/EnemyBaseCharacter.h"
#include "Enemy/EnemyGruntCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Level/EnemySpawner.h"

AWaveManager::AWaveManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWaveManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWaveManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWaveManager::SpawnWave()
{
	AEnemySpawner* EnemySpawner = Cast<AEnemySpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemySpawner::StaticClass()));

	if(EnemySpawner)
	{
		EnemySpawner->SpawnEnemies();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid actor"))
	}

	TArray<AActor*> FoundActors;
	TArray<AEnemyGruntCharacter*> Enemies;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyGruntCharacter::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (AEnemyGruntCharacter* Grunt = Cast<AEnemyGruntCharacter>(Actor))
		{
			Enemies.Add(Grunt);
			Grunt->OnDeath.AddUObject(this, &AWaveManager::HandleEnemyDeath);
			NumOfEnemiesInWave += 1;
			
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Size: %d"), FoundActors.Num());
	UE_LOG(LogTemp, Warning, TEXT("Death"));
	
}

void AWaveManager::HandleEnemyDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Death"));
	// UE_LOG(LogTemp, Warning, TEXT("Num of enem: %d"), NumOfEnemiesInWave.Num());
	
	NumOfEnemiesInWave -= 1;
	if(NumOfEnemiesInWave == 0)
	{
		NumOfEnemiesInWave = 0;
		SpawnWave();
	}
}

