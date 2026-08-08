#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeExecutionTypes.h"
#include "STT_PerformAbility.generated.h"

class UAbilitySystemComponent;

USTRUCT()
struct FROMTHEASHESREBORN_API FStateTreeTask_PerformAbility_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> InputActor = nullptr;

	UPROPERTY(EditAnywhere, Category = Input)
	FGameplayTag EventTag = FGameplayTag::EmptyTag;
	
};

USTRUCT(DisplayName = "Perform Ability")
struct FROMTHEASHESREBORN_API FStateTreeTask_PerformAbility : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	

	using FInstanceDataType = FStateTreeTask_PerformAbility_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	EStateTreeRunStatus PerformAbility(const FStateTreeExecutionContext& Context) const;

private:

	UPROPERTY()
	mutable UAbilitySystemComponent* ASC = nullptr;
	
	mutable FGameplayTag FinishedTag;
	
	mutable FDelegateHandle TagDelegateHandle;
	mutable bool IsTaskFinished = false;

	void FinishTask(FGameplayTag Tag, int32 NewCount) const;

};
