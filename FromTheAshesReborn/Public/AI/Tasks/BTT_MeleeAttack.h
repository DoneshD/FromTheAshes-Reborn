// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_MeleeAttack.generated.h"


UCLASS()
class FROMTHEASHESREBORN_API UBTT_MeleeAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector AttackRadiusKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector IdealRangeKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementSpeed MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tokens")
	int TokensNeeded;

private:
	UBehaviorTreeComponent* EnemyOwnerComp;

public:

	UBTT_MeleeAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//UFUNCTION()
	//void OnMoveCompleted(FAIRequestID RequestID, FPathFollowingResult Result);

	//UFUNCTION()
	// //virtual void OnMoveCompleted(FAIRequestID RequestID, FPathFollowingResult Result) override;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	void FinishedAttacking();

};
