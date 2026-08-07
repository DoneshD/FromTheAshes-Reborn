

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_ExtractStateTreeParams.generated.h"


class UAICombatParameters;
class AAIController;

USTRUCT()
struct FROMTHEASHESREBORN_API FStateTreeTask_ExtractStateTreeParams_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UAICombatParameters> CombatParameters;
	
	UPROPERTY(EditAnywhere, Category = Output)
	float TestAcceptableRadius;
	
};

USTRUCT(DisplayName = "Extract State Tree Params")
struct FROMTHEASHESREBORN_API FStateTreeTask_ExtractStateTreeParams : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeTask_ExtractStateTreeParams_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	
};