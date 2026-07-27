#include "Level/WaveManager.h"

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

