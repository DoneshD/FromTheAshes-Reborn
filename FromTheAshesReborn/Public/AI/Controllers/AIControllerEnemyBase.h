#pragma once

#include "CoreMinimal.h"
#include "Enums/EAIStates.h"
#include "Enums/EAISenses.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIController.h"
#include "AIControllerEnemyBase.generated.h"

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
	FName OrbitRadiusKeyName = TEXT("OrbitRadius");

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> AttackTarget;

	float AttackRadius = 0.0f;
	float DefendRadius = 0.0f;
	float OrbitRadius = 0.0f;

	UPROPERTY(EditAnywhere)
	EAIStates PriorState;

	AAIControllerEnemyBase(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	
private:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAISenseConfig_Sight> AISenseConfigSight;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAISenseConfig_Hearing> AISenseConfigHearing;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAISenseConfig_Damage> AISenseConfigDamage;

public:

	FAIStimulus OutSightStimuliInfo;
	FAIStimulus OutHearStimuliInfo;
	FAIStimulus OutDamageStimuliInfo;

	EAIStates GetCurrentState();

	UFUNCTION(BlueprintCallable)
	void SetStateAsPassive();

	UFUNCTION(BlueprintCallable)
	void SetStateAsAttacking(AActor* IncomingAttackTarget, bool UseLastKnownAttackTarget);

	void SetStateAsOrbiting(AActor* IncomingAttackTarget);

	void SetStateAsInvestigating(FVector Location);

	void SetStateAsFrozen();

	void SetStateAsDead();

	const FAIStimulus CanSenseActor(AActor* Actor, EAISenses Sense);

	void HandleSensedSight(AActor* Actor);

	void HandleSensedSound(FVector Location);

	void HandleSensedDamage(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void ResumePriorState(AActor* CurrentAttackTarget);

};
