// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Enums/EMovementSpeed.h"
#include "AIEnemyInterface.generated.h"

// This class does not need to be modified.


UINTERFACE(MinimalAPI)
class UAIEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FROMTHEASHESREBORN_API IAIEnemyInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual float SetMovementSpeed(EMovementSpeed SpeedState) = 0;

	UFUNCTION()
	virtual void GetIdealRange(float& OutAttackRadius, float& OutDefendRadius) = 0;

	UFUNCTION()
	virtual void JumpToDestination(FVector Destination) = 0;

	UFUNCTION()
	virtual void LightAttack() = 0;

	UFUNCTION()
	virtual bool AttackStart(AActor* AttackTarget, int TokensNeeded) = 0;

	UFUNCTION()
	virtual void AttackEnd(AActor* AttackTarget) = 0;

	UFUNCTION()
	virtual void StoreAttackTokens(AActor* AttackTarget, int Amount) = 0;

	UFUNCTION()
	virtual void OnTargeted() = 0;

	UFUNCTION()
	virtual void EndTargeted() = 0;

};