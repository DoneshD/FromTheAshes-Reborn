// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/EAIStates.h"
#include "AIController.h"
#include "AIControllerEnemyBase.generated.h"

/**
 * 
 */

class UBehaviorTree;
class UBlackboardComponent;
UCLASS()
class FROMTHEASHESREBORN_API AAIControllerEnemyBase : public AAIController
{
	GENERATED_BODY()

public:
	FName StateKeyName = TEXT("State");
	FName AttackTargetKeyName = TEXT("AttackTarget");
	FName PointOfInterestKeyName = TEXT("PointOfInterest");

	AActor* AttackTarget;

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> BaseBehaviorTree;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBlackboardComponent> BaseBlackboardComponent;


public:


	EAIStates GetCurrentState();

	void SetStateAsPassive();

	void SetStateAsAttacking(AActor* IncomingAttackTarget, bool UseLastKnownAttackTarget);

	void SetStateAsInvestigating(FVector Location);

	void SetStateAsFrozen();

	void SetStateAsDead();

	
};
