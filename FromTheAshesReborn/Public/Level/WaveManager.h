#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"

class AFTAGameModeBase;

UCLASS()
class FROMTHEASHESREBORN_API AWaveManager : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AFTAGameModeBase> GameModeClass;


protected:
	
	AWaveManager();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:

	int NumOfEnemiesInWave = 0;
	
	UFUNCTION(BlueprintCallable)
	void SpawnWave();

	void HandleEnemyDeath();
};
