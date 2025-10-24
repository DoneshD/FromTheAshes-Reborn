#pragma once
#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "STC_CheckTargetAttackTokens.generated.h"


USTRUCT()
struct FROMTHEASHESREBORN_API FStateTreeCondition_CheckTargetAttackTokens_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> Target = nullptr;

	UPROPERTY(EditAnywhere)
	int32 AmountToCheck = 1;
	
};

USTRUCT(DisplayName = "Check Target Attack Tokens")
struct FROMTHEASHESREBORN_API FStateTreeCondition_CheckTargetAttackTokens : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeCondition_CheckTargetAttackTokens_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
	
};