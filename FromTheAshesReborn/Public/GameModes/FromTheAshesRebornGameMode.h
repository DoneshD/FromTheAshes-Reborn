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

	TObjectPtr<AFromTheAshesRebornGameMode> FTAGameMode;

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

	TArray<AActor*> EnemiesArray;

	TSubclassOf<AEnemyBase> EnemyBaseClass = AEnemyBase::StaticClass();

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACombatManager> CombatManagerClass;

	ACombatManager* CombatManager;

	FVector SpawnerLocation;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParams;

	int EnemiesCount = 0;

	UPROPERTY(BlueprintReadOnly)
	bool IsAggroActivated = false;

	bool IsPlayerInvincible = false;

	bool IsAllInvicible = false;


public:

	AFromTheAshesRebornGameMode();

	virtual void BeginPlay() override;

	virtual void PublishEnemySpawned() override;

	virtual void PublishEnemyDeath() override;

	virtual void SpawnMeleeEnemy() override;

	virtual void ToggleEnemyAggro() override;

	virtual void TogglePlayerInvincibility() override;

	virtual void ToggleAllInvincibility() override;

	UFUNCTION(BlueprintCallable)
	virtual void HandleAttackRequest(AActor* Attacker) override;

	virtual void HandleDeath(AActor* Attacker) override;

	void IncrementEnemyCount();

	void CheckEnemyCount();

};