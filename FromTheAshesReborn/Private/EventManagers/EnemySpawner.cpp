#include "EventManagers/EnemySpawner.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
    SpawnRadius = 500.0f;

}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


FVector AEnemySpawner::GetRandomLocationAroundSpawner(float Radius)
{
    FVector Origin = GetActorLocation();
    FVector RandomLocation;

    float Angle = FMath::RandRange(0.0f, 360.0f);
    float Distance = FMath::RandRange(0.0f, Radius);

    RandomLocation.X = Origin.X + FMath::Cos(FMath::DegreesToRadians(Angle)) * Distance;
    RandomLocation.Y = Origin.Y + FMath::Sin(FMath::DegreesToRadians(Angle)) * Distance;
    RandomLocation.Z = Origin.Z; 

    return RandomLocation;
}

