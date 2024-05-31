#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/EnemySpawnerInterface.h"
#include "EnemySpawnerInteractable.generated.h"

UENUM(BlueprintType)
enum class ESpawnerInteractable : uint8
{
	ESpawnerInteractable_AddEnemy				UMETA(DisplayName = "AddEnemy"),
	ESpawnerInteractable_ToggleAttacking		UMETA(DisplayName = "ToggleAggro"),
	ESpawnerInteractable_PlayerInvincible		UMETA(DisplayName = "PlayerInvincible"),
	ESpawnerInteractable_PlayerEnemyInvincible	UMETA(DisplayName = "PlayerEnemyInvincible")
};

UCLASS()
class FROMTHEASHESREBORN_API AEnemySpawnerInteractable : public AActor, public IEnemySpawnerInterface
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditAnywhere)
	ESpawnerInteractable SpawnerInteractable;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	TObjectPtr<AEnemyBase> EnemyBase;
	TSubclassOf<class AEnemyBase> EnemyBaseClass;

	FActorSpawnParameters SpawnParams;

	bool isActive = false;

public:	
	AEnemySpawnerInteractable();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnHit() override;

	//void SetSpawnerInteractble();

};
