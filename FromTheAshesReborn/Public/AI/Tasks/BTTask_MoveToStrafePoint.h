// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MoveToStrafePoint.generated.h"

/**
 * 
 */
UCLASS()
class FROMTHEASHESREBORN_API UBTTask_MoveToStrafePoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector PointOfInterestKey;

	UBehaviorTreeComponent* EnemyOwnerComp;

public:

	UBTTask_MoveToStrafePoint();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void ReachedLocation();
};

