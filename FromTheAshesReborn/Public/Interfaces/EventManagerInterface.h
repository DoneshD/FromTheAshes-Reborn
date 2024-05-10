#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EventManagerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UEventManagerInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IEventManagerInterface
{
	GENERATED_BODY()

public:

	virtual void PublishEnemySpawned() = 0;

	virtual void PublishEnemyDeath() = 0;

};
