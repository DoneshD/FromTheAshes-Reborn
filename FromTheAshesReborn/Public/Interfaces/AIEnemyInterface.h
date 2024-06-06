#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Enums/EMovementSpeed.h"
#include "../Enums/EAIStates.h"
#include "AIEnemyInterface.generated.h"

UINTERFACE(MinimalAPI)
class UAIEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IAIEnemyInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual float SetMovementSpeed(EMovementSpeed SpeedState) = 0;

	UFUNCTION()
	virtual void GetIdealRange(float& OutAttackRadius, float& OutDefendRadius, float& OutOrbitRadius) = 0;

	UFUNCTION()
	virtual void JumpToDestination(FVector Destination) = 0;

	UFUNCTION()
	virtual void Retreat() = 0;

	UFUNCTION()
	virtual void LightAttack() = 0;

	virtual EAIStates GetCurrentState() = 0;

	virtual void SetStateAsAttacking(TObjectPtr<AActor> AttackTarget) = 0;

	virtual void SetStateAsOrbiting(TObjectPtr<AActor> AttackTarget) = 0;

};