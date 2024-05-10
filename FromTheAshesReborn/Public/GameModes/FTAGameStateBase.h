#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FTAGameStateBase.generated.h"

DECLARE_DELEGATE(FOnEnemyDeath)

UCLASS()
class FROMTHEASHESREBORN_API AFTAGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	FOnEnemyDeath OnEnemyDeath;

public:

};
