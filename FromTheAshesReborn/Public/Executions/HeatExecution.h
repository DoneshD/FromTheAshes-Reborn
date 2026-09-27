#pragma once
#include "GameplayEffectExecutionCalculation.h"
#include "HeatExecution.generated.h"

class UObject;

UCLASS()
class UHeatExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UHeatExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
