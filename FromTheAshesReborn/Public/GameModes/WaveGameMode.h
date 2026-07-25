#pragma once

#include "CoreMinimal.h"
#include "FTAGameModeBase.h"
#include "WaveGameMode.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API AWaveGameMode : public AFTAGameModeBase
{
	GENERATED_BODY()

public:
	
	AWaveGameMode();
	
	virtual void BeginPlay() override;
};
