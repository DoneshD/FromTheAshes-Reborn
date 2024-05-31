#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/EventManagerInterface.h"
#include "FromTheAshesRebornGameMode.generated.h"

class AEnemyBase;

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

	int EnemiesCount = 0;


public:

	AFromTheAshesRebornGameMode();

	virtual void BeginPlay() override;

	virtual void PublishEnemySpawned() override;

	virtual void PublishEnemyDeath() override;

	void IncrementEnemyCount();

	void CheckEnemyCount();

};