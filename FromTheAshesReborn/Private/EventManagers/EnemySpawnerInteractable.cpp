#include "EventManagers/EnemySpawnerInteractable.h"
#include "Characters/EnemyBase.h"

AEnemySpawnerInteractable::AEnemySpawnerInteractable()
{
	PrimaryActorTick.bCanEverTick = true;

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshMesh"));
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

        break;

    case ESpawnerInteractable::ESpawnerInteractable_ToggleAttacking:
        break;

    case ESpawnerInteractable::ESpawnerInteractable_PlayerInvincible:
        break;

    case ESpawnerInteractable::ESpawnerInteractable_PlayerEnemyInvincible:
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Unknown interactable type"));
        break;
    }
}

