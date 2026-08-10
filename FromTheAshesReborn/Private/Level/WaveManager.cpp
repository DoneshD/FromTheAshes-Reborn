#include "Level/WaveManager.h"

#include "DataAsset/EnemyEncounterDataAsset.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "GameModes/FTAGameModeBase.h"
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
	//only works with one spawner per level right now
	AFTAGameModeBase* FTAGameMode = Cast<AFTAGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if(!FTAGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("AWaveManager::SpawnWave() - Invalid gamemode"));
		return;
	}
	
	AEnemySpawner* EnemySpawner = Cast<AEnemySpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemySpawner::StaticClass()));

	if(!EnemySpawner)
	{
		UE_LOG(LogTemp, Error, TEXT("AWaveManager::SpawnWave() - Invalid EnemySpawner"));
		return;
	}
	// FWaveData WaveData= FTAGameMode->EnemyEncounterArray[FTAGameMode->CurrentEncounter];
	FWaveData WaveData = FTAGameMode->EnemyEncounterArray[FTAGameMode->CurrentEncounter]->WaveData;
	
	EnemySpawner->SpawnEnemies(WaveData);
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBaseCharacter::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (AEnemyBaseCharacter* Enemy = Cast<AEnemyBaseCharacter>(Actor))
		{
			Enemy->OnDeath.AddUObject(this, &AWaveManager::HandleEnemyDeath);
			NumOfEnemiesInWave += 1;
		}
	}
	
}

void AWaveManager::HandleEnemyDeath()
{
	NumOfEnemiesInWave -= 1;
	if(NumOfEnemiesInWave == 0)
	{
		NumOfEnemiesInWave = 0;
		AFTAGameModeBase* FTAGameMode = Cast<AFTAGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		if(!FTAGameMode)
		{
			UE_LOG(LogTemp, Error, TEXT("AWaveManager::HandleEnemyDeath() - Invalid gamemode"));
			return;
		}

		FTAGameMode->CurrentEncounter += 1;
		if(FTAGameMode->CurrentEncounter >= FTAGameMode->EnemyEncounterArray.Num())
		{
			FTAGameMode->CurrentEncounter = FTAGameMode->EnemyEncounterArray.Num() - 1;
		}
		
		SpawnWave();
	}
}

