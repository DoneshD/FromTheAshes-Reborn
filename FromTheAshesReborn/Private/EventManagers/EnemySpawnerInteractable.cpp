#include "EventManagers/EnemySpawnerInteractable.h"

AEnemySpawnerInteractable::AEnemySpawnerInteractable()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemySpawnerInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemySpawnerInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawnerInteractable::OnHit()
{
    switch (SpawnerInteractable)
    {
    case ESpawnerInteractable::ESpawnerInteractable_AddEnemy:
        UE_LOG(LogTemp, Warning, TEXT("AddEnemy selected"));
        break;

    case ESpawnerInteractable::ESpawnerInteractable_ToggleAttacking:
        UE_LOG(LogTemp, Warning, TEXT("ToggleAggro selected"));
        break;

    case ESpawnerInteractable::ESpawnerInteractable_PlayerInvincible:
        UE_LOG(LogTemp, Warning, TEXT("PlayerInvincible selected"));
        break;

    case ESpawnerInteractable::ESpawnerInteractable_PlayerEnemyInvincible:
        UE_LOG(LogTemp, Warning, TEXT("PlayerEnemyInvincible selected"));
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Unknown interactable type"));
        break;
    }
}

