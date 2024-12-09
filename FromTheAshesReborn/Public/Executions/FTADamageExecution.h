#pragma once
#include "GameplayEffectExecutionCalculation.h"
#include "FTADamageExecution.generated.h"

class UObject;

UCLASS()
class UFTADamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UFTADamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
