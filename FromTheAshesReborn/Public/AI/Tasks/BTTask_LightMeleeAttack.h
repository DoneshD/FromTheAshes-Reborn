// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_LightMeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class FROMTHEASHESREBORN_API UBTTask_LightMeleeAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

private:
	bool bDoneAttacking = true;
protected:

	UBTTask_LightMeleeAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void FinsihedAttacking();

};
