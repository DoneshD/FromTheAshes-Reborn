#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interfaces/EventManagerInterface.h"
#include "FTAGameStateBase.generated.h"

DECLARE_DELEGATE(FOnEnemySpawned)
DECLARE_DELEGATE(FOnEnemyDeath)

UCLASS()
class FROMTHEASHESREBORN_API AFTAGameStateBase : public AGameStateBase, public IEventManagerInterface
{
	GENERATED_BODY()
	
public:

	FOnEnemySpawned OnEnemySpawned;

	FOnEnemyDeath OnEnemyDeath;

public:

	virtual void PublishEnemySpawned() override;

	virtual void PublishEnemyDeath() override;

};
