#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"
#include "STE_GetMinimumCombatRangeLocation.generated.h"

USTRUCT()
struct FROMTHEASHESREBORN_API FStateTreeEvaluator_GetMinimumCombatRangeLocation_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> OwningActor = nullptr;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere)
	float DistanceToTarget = 0.0f;

	UPROPERTY(EditAnywhere, Category = Output)
	FVector MinimumLocation;
	
};

USTRUCT(DisplayName = "Get Minimum Combat Range Location")
struct FROMTHEASHESREBORN_API FStateTreeEvaluator_GetMinimumCombatRangeLocation : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeEvaluator_GetMinimumCombatRangeLocation_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

protected:
	
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void TreeStop(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
};
