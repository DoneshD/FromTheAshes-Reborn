#pragma once
#include "GameplayEffectExecutionCalculation.h"
#include "ArmorExecution.generated.h"

class UObject;

UCLASS()
class UArmorExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UArmorExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
