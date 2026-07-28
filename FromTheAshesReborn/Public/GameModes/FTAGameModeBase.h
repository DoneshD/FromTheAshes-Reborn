#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FTAGameModeBase.generated.h"

class AEnemyBaseCharacter;
struct FWaveData;

USTRUCT(BlueprintType)
struct FWaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<TSubclassOf<AEnemyBaseCharacter>, int32> EnemySpawnCount;
};

UCLASS()
class FROMTHEASHESREBORN_API AFTAGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	int32 CurrentEncounter = 0;

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<FWaveData> EnemyEncounterArray;

public:
	AFTAGameModeBase();
	
	virtual void BeginPlay() override;
	
};
