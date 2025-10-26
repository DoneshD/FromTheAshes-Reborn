#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_ReserveAttackTokens.generated.h"

USTRUCT()
struct FROMTHEASHESREBORN_API FStateTreeTask_ReserveAttackTokens_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> Target = nullptr;

	UPROPERTY(EditAnywhere)
	int32 TokenAmount = 1;
	
};

USTRUCT(DisplayName = "Reserve Attack Tokens")
struct FROMTHEASHESREBORN_API FStateTreeTask_ReserveAttackTokens : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeTask_ReserveAttackTokens_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
};