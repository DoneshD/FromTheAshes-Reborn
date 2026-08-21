#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "EnemySpawner.generated.h"


class UEnemyCharacterDataAsset;
class AAIControllerEnemyBase;
class AEnemyBaseCharacter;
struct FWaveData;

UCLASS()
class FROMTHEASHESREBORN_API AEnemySpawner : public ATriggerBox
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VerticalLocationOffset = 100.0f;

protected:
	
	AEnemySpawner();
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

public:

	UFUNCTION(BlueprintCallable)
	void SpawnEnemies(FWaveData InWaveData);

	UFUNCTION(BlueprintCallable)
	void ConstructStateTree(AAIControllerEnemyBase* EnemyController, UEnemyCharacterDataAsset* EnemyData, AEnemyBaseCharacter* EnemyChar);

};
