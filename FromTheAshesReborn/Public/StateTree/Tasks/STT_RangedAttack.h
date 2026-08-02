#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeExecutionTypes.h"
#include "STT_RangedAttack.generated.h"

class UAbilitySystemComponent;

USTRUCT()
struct FROMTHEASHESREBORN_API FStateTreeTask_RangedAttack_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> InputActor = nullptr;

	UPROPERTY(EditAnywhere, Category = Input)
	FGameplayTag StateTreeStartedTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(EditAnywhere, Category = Input)
	FGameplayTag StateTreeRunningTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(EditAnywhere, Category = Input)
	FGameplayTag StateTreeFinishedTag = FGameplayTag::EmptyTag;
	
};

USTRUCT(DisplayName = "Ranged Attack")
struct FROMTHEASHESREBORN_API FStateTreeTask_RangedAttack : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	

	using FInstanceDataType = FStateTreeTask_RangedAttack_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	EStateTreeRunStatus ActivateRangedAttack(const FStateTreeExecutionContext& Context) const;

private:

	mutable UAbilitySystemComponent* ASC;
	
	mutable FGameplayTag FinishedTag;
	
	mutable FDelegateHandle TagDelegateHandle;
	mutable bool IsTaskFinished = false;

	void FinishTask(FGameplayTag Tag, int32 NewCount) const;

};
