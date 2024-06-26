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

	virtual void SpawnMeleeEnemy() = 0;

	virtual void ToggleEnemyAggro() = 0;

	virtual void TogglePlayerInvincibility() = 0;

	virtual void ToggleAllInvincibility() = 0;

	virtual void HandleAttackRequest(AActor* Attacker) = 0;

	virtual void HandleAttackerSwapRequest(AActor* Attacker) = 0;

	virtual void HandleDeath(AActor* Attacker) = 0;

};
