// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_IsHealthBelowThreshold.generated.h"

/**
 * 
 */
UCLASS()
class FROMTHEASHESREBORN_API UBTD_IsHealthBelowThreshold : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	float HealthThreshold = 0.3f;

public:

	UBTD_IsHealthBelowThreshold();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
