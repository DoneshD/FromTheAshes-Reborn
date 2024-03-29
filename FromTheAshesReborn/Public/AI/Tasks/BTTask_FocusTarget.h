// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Enums/EMovementSpeed.h"
#include "BTTask_FocusTarget.generated.h"

/**
 * 
 */
UCLASS()
class FROMTHEASHESREBORN_API UBTTask_FocusTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector AttackTargetKey;

public:

	UBTTask_FocusTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
