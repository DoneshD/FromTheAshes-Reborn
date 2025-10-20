#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "STC_IsTargetInIdeaRange.generated.h"

USTRUCT()
struct FROMTHEASHESREBORN_API FStateTreeCondition_IsTargetInIdeaRange_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	float DistanceBetweenActors;

	UPROPERTY(EditAnywhere)
	float AcceptableRadius = 300.0f;
	
};

USTRUCT(DisplayName = "Is Target In Ideal Range")
struct FROMTHEASHESREBORN_API FStateTreeCondition_IsTargetInIdeaRange : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeCondition_IsTargetInIdeaRange_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
	
};