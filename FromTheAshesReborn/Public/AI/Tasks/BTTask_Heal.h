// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Heal.generated.h"

/**
 * 
 */
UCLASS()
class FROMTHEASHESREBORN_API UBTTask_Heal : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	float HealPercentage = 0.3f;

public:

	UBTTask_Heal();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};