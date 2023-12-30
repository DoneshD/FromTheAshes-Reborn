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
class UAISenseConfig_Damage;

UCLASS()
class FROMTHEASHESREBORN_API AAIControllerEnemyBase : public AAIController
{
	GENERATED_BODY()

public:
	FName StateKeyName = TEXT("State");
	FName AttackTargetKeyName = TEXT("AttackTarget");
	FName PointOfInterestKeyName = TEXT("PointOfInterest");
	FName AttackRadiusKeyName = TEXT("AttackRadius");
	FName DefendRadiusKeyName = TEXT("DefendRadius");

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> AttackTarget;
	float AttackRadius = 0.0f;
	float DefendRadius = 0.0f;

	AAIControllerEnemyBase();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void TargetActorsPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
private:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBlackboardComponent> BaseBlackboardComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAISenseConfig_Sight> AISenseConfigSight;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAISenseConfig_Hearing> AISenseConfigHearing;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAISenseConfig_Damage> AISenseConfigDamage;


public:

	FAIStimulus OutSightInfo;
	FAIStimulus OutHearInfo;
	FAIStimulus OutDamageInfo;

	EAIStates GetCurrentState();

	void SetStateAsPassive();

	void SetStateAsAttacking(AActor* IncomingAttackTarget, bool UseLastKnownAttackTarget);

	void SetStateAsInvestigating(FVector Location);

	void SetStateAsFrozen();

	void SetStateAsDead();

	bool CanSenseActor(AActor* Actor, EAISenses Sense, FAIStimulus& OutStimuli);

	void HandleSensedSight(AActor* Actor);

	void HandleSensedSound(FVector Location);

	void HandleSensedDamage(AActor* Actor);

};
