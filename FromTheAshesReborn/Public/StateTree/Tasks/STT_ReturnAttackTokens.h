#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_ReturnAttackTokens.generated.h"

USTRUCT()
struct FROMTHEASHESREBORN_API FStateTreeTask_ReturnAttackTokens_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> InputActor = nullptr;
	
};

USTRUCT(DisplayName = "Return Attack Tokens")
struct FROMTHEASHESREBORN_API FStateTreeTask_ReturnAttackTokens : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeTask_ReturnAttackTokens_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
};