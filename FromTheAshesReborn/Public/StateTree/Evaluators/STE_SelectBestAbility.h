#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"
#include "DataAsset/AICombatParameters.h"
#include "STE_SelectBestAbility.generated.h"

class UEnemyCharacterDataAsset;
class UFTAAbilitySet;
struct FAttackData;
class UFTAGameplayAbility;
class UFTAAbilityDataAsset;

USTRUCT()
struct FROMTHEASHESREBORN_API FStateTreeEvaluator_SelectBestAbility_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> InputActor = nullptr;

	// UPROPERTY(EditAnywhere, Category = Input)
	// TObjectPtr<UEnemyCharacterDataAsset> EnemyData = nullptr;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, Category = Input)
	TArray<TObjectPtr<UFTAAbilitySet>> AbilitySets;

	UPROPERTY(EditAnywhere, Category = Input)
	float DistanceToTarget = 1.0f;

	UPROPERTY(EditAnywhere, Category = Output)
	TObjectPtr<UFTAAbilityDataAsset> AbilityAsset = nullptr;

	UPROPERTY()
	TObjectPtr<UFTAGameplayAbility> BestSelectedAbility = nullptr;
	
};

USTRUCT(DisplayName = "Select Best Ability")
struct FROMTHEASHESREBORN_API FStateTreeEvaluator_SelectBestAbility : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeEvaluator_SelectBestAbility_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

protected:
	
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void TreeStop(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	UFTAGameplayAbility* WeightedRandomSelection(TArray<TObjectPtr<UFTAGameplayAbility>> AbilityArray, float DistToTarget) const;


	
	
};