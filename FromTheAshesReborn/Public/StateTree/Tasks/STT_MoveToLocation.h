#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_MoveToLocation.generated.h"

class AAIController;

USTRUCT()
struct FROMTHEASHESREBORN_API FStateTreeTask_MoveToLocation_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> InputActor = nullptr;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY(EditAnywhere, Category = Input)
	FVector Location;

	UPROPERTY(EditAnywhere, Category = Input)
	float AcceptableRadius = 300.0f;
	
};

USTRUCT(DisplayName = "Move To Location")
struct FROMTHEASHESREBORN_API FStateTreeTask_MoveToLocation : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeTask_MoveToLocation_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	
};