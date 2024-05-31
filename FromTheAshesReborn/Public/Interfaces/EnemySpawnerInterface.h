#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemySpawnerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UEnemySpawnerInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IEnemySpawnerInterface
{
	GENERATED_BODY()

public:

	virtual void OnHit() = 0;
};
