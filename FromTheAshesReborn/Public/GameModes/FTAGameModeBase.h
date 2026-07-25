#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FTAGameModeBase.generated.h"

class AEnemyBaseCharacter;

UCLASS()
class FROMTHEASHESREBORN_API AFTAGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFTAGameModeBase();
	
	virtual void BeginPlay() override;
	
};
