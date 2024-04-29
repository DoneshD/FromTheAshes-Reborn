#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Enums/EMovementSpeed.h"
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
	virtual bool AttackStart(AActor* AttackTarget, int TokensNeeded) = 0;

	UFUNCTION()
	virtual void AttackEnd(AActor* AttackTarget) = 0;

	UFUNCTION()
	virtual void StoreAttackTokens(AActor* AttackTarget, int Amount) = 0;

	UFUNCTION()
	virtual void LightAttack() = 0;

	virtual void Attack(TObjectPtr<AActor> AttackTarget) = 0;

	virtual void Wait(TObjectPtr<AActor> AttackTarget) = 0;
};