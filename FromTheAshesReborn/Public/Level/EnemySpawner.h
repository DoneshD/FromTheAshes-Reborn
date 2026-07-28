#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "EnemySpawner.generated.h"

struct FWaveData;
class AAIControllerEnemyGrunt;
class AEnemyGruntCharacter;

UCLASS()
class FROMTHEASHESREBORN_API AEnemySpawner : public ATriggerBox
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VerticalLocationOffset = 100.0f;

protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyGruntCharacter> GruntClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAIControllerEnemyGrunt> GruntControllerClass;

protected:
	
	AEnemySpawner();
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

public:

	UFUNCTION(BlueprintCallable)
	void SpawnEnemies(FWaveData InWaveData);

};
