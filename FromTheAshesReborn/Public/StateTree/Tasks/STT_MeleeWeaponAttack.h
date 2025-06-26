#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_MeleeWeaponAttack.generated.h"

USTRUCT()
struct FStateTreeTask_MeleeWeaponAttack_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag EventTag;
	
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag StateTreeStartedTag;

	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag StateTreeRunningTag;
	
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag StateTreeFinishedTag;
};

USTRUCT(DisplayName = "Melee Weapon Attack")
struct FROMTHEASHESREBORN_API FStateTreeTask_MeleeWeaponAttack : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	FDelegateHandle EventHandle;
	bool bReceivedFinishEvent = false;

	using FInstanceDataType = FStateTreeTask_MeleeWeaponAttack_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

private:
	EStateTreeRunStatus ActivateMeleeAttack(const FStateTreeExecutionContext& Context) const;
};
