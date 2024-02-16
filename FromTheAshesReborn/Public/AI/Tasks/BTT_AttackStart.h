// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_AttackStart.generated.h"

/**
 * 
 */
UCLASS()
class FROMTHEASHESREBORN_API UBTT_AttackStart : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tokens")
	int TokensNeeded = 1;
	

public:

	UBTT_AttackStart();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};