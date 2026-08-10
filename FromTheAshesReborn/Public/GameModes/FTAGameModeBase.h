#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FTAGameModeBase.generated.h"

class UEnemyEncounterDataAsset;
class UEnemyCharacterDataAsset;
class AEnemyBaseCharacter;


UCLASS()
class FROMTHEASHESREBORN_API AFTAGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	int32 CurrentEncounter = 0;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<TObjectPtr<UEnemyEncounterDataAsset>> EnemyEncounterArray;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool ActivateAIBehavior = true;

protected:
	
	AFTAGameModeBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	
};
