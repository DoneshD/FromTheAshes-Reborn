#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"
#include "STE_GetTargetLocation.generated.h"

USTRUCT()
struct FROMTHEASHESREBORN_API FStateTreeEvaluator_GetTargetLocation_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, Category = Output)
	FVector TargetLocation;
	
};

USTRUCT(DisplayName = "Get Target Location")
struct FROMTHEASHESREBORN_API FStateTreeEvaluator_GetTargetLocation : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeEvaluator_GetTargetLocation_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

protected:
	
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void TreeStop(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
};
