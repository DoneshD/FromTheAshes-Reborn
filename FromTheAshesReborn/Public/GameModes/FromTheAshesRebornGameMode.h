#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/EventManagerInterface.h"
#include "../Characters/EnemyMelee.h"
#include "../EventManagers/EnemySpawner.h"
#include "FromTheAshesRebornGameMode.generated.h"

class AEnemyMelee;
class AEnemySpawner;
class AAIControllerEnemyBase;

DECLARE_DELEGATE(FOnEnemySpawned)
DECLARE_DELEGATE(FOnEnemyDeath)

UCLASS(minimalapi)
class AFromTheAshesRebornGameMode : public AGameModeBase, public IEventManagerInterface
{
	GENERATED_BODY()

public:

	TObjectPtr<AActor> HardTarget;

	FOnEnemySpawned OnEnemySpawned;

	FOnEnemyDeath OnEnemyDeath;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemySpawner> EnemySpawnerClass;
	AEnemySpawner* EnemySpawner;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemyMelee> EnemyMeleeClass;
	TObjectPtr<AEnemyMelee> EnemyMelee;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AAIControllerEnemyBase> AIControllerEnemyBaseClass;
	TObjectPtr<AAIControllerEnemyBase> AIControllerEnemyBase;

	FVector SpawnerLocation;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParams;

	int EnemiesCount = 0;


public:

	AFromTheAshesRebornGameMode();

	virtual void BeginPlay() override;

	virtual void PublishEnemySpawned() override;

	virtual void PublishEnemyDeath() override;

	virtual void SpawnMeleeEnemy() override;

	void IncrementEnemyCount();

	void CheckEnemyCount();

};