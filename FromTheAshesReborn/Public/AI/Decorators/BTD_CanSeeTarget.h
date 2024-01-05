// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_CanSeeTarget.generated.h"

/**
 * 
 */
UCLASS()
class FROMTHEASHESREBORN_API UBTD_CanSeeTarget : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector AttackTargetKey;

public:
	
	UBTD_CanSeeTarget();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
