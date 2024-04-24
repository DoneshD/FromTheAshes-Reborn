// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MoveToIdealRange.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UBTTask_MoveToIdealRange : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector IdealRangeKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementSpeed MovementSpeed;

public:

	UBTTask_MoveToIdealRange();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void ReachedLocation(FAIRequestID RequestID, const FPathFollowingResult& Result);
};
