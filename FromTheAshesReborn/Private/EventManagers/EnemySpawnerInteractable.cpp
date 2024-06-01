#include "EventManagers/EnemySpawnerInteractable.h"
#include "Kismet/GameplayStatics.h"
#include "EventManagers/EnemySpawner.h"
#include "Interfaces/EventManagerInterface.h"
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
    IEventManagerInterface* EventManagerInterface = Cast<IEventManagerInterface>(UGameplayStatics::GetGameMode(GetWorld()));

    switch (SpawnerInteractable)
    {
    
    case ESpawnerInteractable::ESpawnerInteractable_AddEnemy:
        EventManagerInterface->SpawnMeleeEnemy();
        break;

    case ESpawnerInteractable::ESpawnerInteractable_ToggleAttacking:
        EventManagerInterface->ToggleEnemyAggro();
        break;

    case ESpawnerInteractable::ESpawnerInteractable_PlayerInvincible:
        EventManagerInterface->TogglePlayerInvincibility();
        break;

    case ESpawnerInteractable::ESpawnerInteractable_PlayerEnemyInvincible:
        EventManagerInterface->ToggleAllInvincibility();
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Unknown interactable type"));
        break;
    }
}

