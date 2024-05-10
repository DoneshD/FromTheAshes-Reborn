#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "FTALevelScriptActor.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API AFTALevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:

	int EnemiesCount = 0;

	TArray<AActor*> EnemiesArray;

	TSubclassOf<AEnemyBase> EnemyBaseClass = AEnemyBase::StaticClass();

	AFTAGameStateBase* FTAGameStateBase;

public:
	
	virtual void BeginPlay() override;

	void CheckEnemyCount();

};
