
#include "GameModes/FromTheAshesRebornGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/EnemyBase.h"
#include "UObject/ConstructorHelpers.h"

AFromTheAshesRebornGameMode::AFromTheAshesRebornGameMode()
{
	// // set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_Azrael"));
	// if (PlayerPawnBPClass.Class != NULL)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }
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
