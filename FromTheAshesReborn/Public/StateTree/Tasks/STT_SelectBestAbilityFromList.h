#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_SelectBestAbilityFromList.generated.h"

class UFTAAbilityDataAsset;
class UFTAGameplayAbility;
class UAICombatParameters;
class AAIController;

USTRUCT()
struct FROMTHEASHESREBORN_API FStateTreeTask_SelectBestAbilityFromList_InstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UFTAGameplayAbility>> PossibleAbilities;

	UPROPERTY(EditAnywhere, Category = Output)
	TObjectPtr<UFTAAbilityDataAsset> SelectedAbilityAsset;
	
};

USTRUCT(DisplayName = "Select Best Ability From List")
struct FROMTHEASHESREBORN_API FStateTreeTask_SelectBestAbilityFromList : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeTask_SelectBestAbilityFromList_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
};