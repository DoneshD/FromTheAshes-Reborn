// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetMovementSpeed.generated.h"

/**
 * 
 */
UCLASS()
class FROMTHEASHESREBORN_API UBTTask_SetMovementSpeed : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementSpeed MovementSpeed;

public:

	UBTTask_SetMovementSpeed();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};