// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/EAIStates.h"
#include "Enums/EAISenses.h"
#include "Perception/AIPerceptionComponent.h"

#include "AIController.h"
#include "AIControllerEnemyBase.generated.h"

/**
 * 
 */

class UBehaviorTree;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

UCLASS()
class FROMTHEASHESREBORN_API AAIControllerEnemyBase : public AAIController
{
	GENERATED_BODY()

public:
	FName StateKeyName = TEXT("State");
	FName AttackTargetKeyName = TEXT("AttackTarget");
	FName PointOfInterestKeyName = TEXT("PointOfInterest");

	TObjectPtr<AActor> AttackTarget;

	AAIControllerEnemyBase();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

	//ActorPerceptionUpdatedDelegate.AddDynamic(this, &AAIControllerEnemyBase::OnPerceptionUpdated);
	//void OnTargetPerceptionUpdated_Delegate(AActor* Actor, FAIStimulus Stimulus);
	
	UFUNCTION()
	virtual void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> BaseBehaviorTree;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBlackboardComponent> BaseBlackboardComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAISenseConfig_Sight> AISenseConfigSight = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAISenseConfig_Hearing> AISenseConfigHearing = nullptr;


public:

	AActor* TestActor;

	EAIStates GetCurrentState();

	void SetStateAsPassive();

	void SetStateAsAttacking(AActor* IncomingAttackTarget, bool UseLastKnownAttackTarget);

	void SetStateAsInvestigating(FVector Location);

	void SetStateAsFrozen();

	void SetStateAsDead();

	bool CanSenseActor(AActor* Actor, EAISenses Sense);

};
