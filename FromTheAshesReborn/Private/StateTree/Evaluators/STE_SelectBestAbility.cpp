#include "StateTree/Evaluators/STE_SelectBestAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "CombatComponents/ComboManagerComponent.h"
#include "Enemy/EnemyCharacterDataAsset.h"
#include "FTAAbilitySystem/AbilitySets/FTAAbilitySet.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_Attack.h"

void FStateTreeEvaluator_SelectBestAbility::TreeStart(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorCommonBase::TreeStart(Context);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	TArray<TObjectPtr<UFTAGameplayAbility>> PossibleAbilities;

	if(InstanceData.AbilitySets.Num() > 0)
	{
		for(auto Set : InstanceData.AbilitySets)
		{
			if(Set)
			{
				if(Set->GrantedGameplayAbilities.Num() > 0)
				{
					for(auto AbilityFromSet : Set->GrantedGameplayAbilities)
					{
						UFTAGameplayAbility* AbilityCDO = AbilityFromSet.Ability->GetDefaultObject<UFTAGameplayAbility>();
						if(AbilityCDO)
						{
							PossibleAbilities.Add(AbilityCDO);
						}
					}
				}
			}
		}
	}

	float Dist = FVector::Dist(InstanceData.TargetActor->GetActorLocation(), InstanceData.InputActor->GetActorLocation());

	if(PossibleAbilities.Num() > 0)
	{
		
		UFTAGameplayAbility* Ability = WeightedRandomSelection(PossibleAbilities, Dist);
		InstanceData.BestSelectedAbility = Ability;
		InstanceData.AbilityAsset = Ability->DefaultAbilityDataAsset;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Possible abilities"))
	}
	
}

void FStateTreeEvaluator_SelectBestAbility::TreeStop(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorCommonBase::TreeStop(Context);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	
}

void FStateTreeEvaluator_SelectBestAbility::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FStateTreeEvaluatorCommonBase::Tick(Context, DeltaTime);
}

UFTAGameplayAbility* FStateTreeEvaluator_SelectBestAbility::WeightedRandomSelection(
	TArray<TObjectPtr<UFTAGameplayAbility>> AbilityArray, float DistToTarget) const
{
	struct FScoringCriteria 
	{
		float FinalScore = 1.0f;
		float DistToTarget = 1.0f;
		float DisRatio = 1.0f;
	};
	
	TMap<UFTAGameplayAbility*, FScoringCriteria> ScoreMap;
	
	UFTAGameplayAbility* BestAbility = AbilityArray[FMath::RandRange(0, AbilityArray.Num() - 1)];

	for(auto Ability : AbilityArray)
	{
		float DistDiff = DistToTarget - Ability->DefaultAbilityDataAsset->AIAbilityData.AcceptableRadiusRange;
		float DistRatio = DistToTarget / Ability->DefaultAbilityDataAsset->AIAbilityData.AcceptableRadiusRange;
		
		FScoringCriteria ScoringCriteria;
		ScoringCriteria.DistToTarget = FMath::Abs(DistDiff);
		ScoringCriteria.DisRatio = FMath::Abs(DistRatio);
		
		ScoreMap.Add(Ability, ScoringCriteria);
	}

	float CurrentLowestRatioDiff = INFINITY;
	for (TPair<UFTAGameplayAbility*, FScoringCriteria>& Pair : ScoreMap)
	{
		FScoringCriteria& Criteria = Pair.Value;

		float RatioDiff = FMath::Abs(Criteria.DisRatio - 1);
		if(RatioDiff < CurrentLowestRatioDiff)
		{
			CurrentLowestRatioDiff = RatioDiff;
			BestAbility = Pair.Key;
		}
		else
		{
			
		}

	}

	for (TPair<UFTAGameplayAbility*, FScoringCriteria>& Pair : ScoreMap)
	{
		UFTAGameplayAbility* Ability = Pair.Key;
		FScoringCriteria& Criteria = Pair.Value;

		if (Ability == BestAbility)
		{
			Criteria.FinalScore = 0.95f;
		}
		else
		{
			Criteria.FinalScore = 0.05f;
		}
	}

	float TotalWeight = 0;

	for (const TPair<UFTAGameplayAbility*, FScoringCriteria>& Pair : ScoreMap)
	{
		TotalWeight += Pair.Value.FinalScore;
	}

	float RandomVal = FMath::FRandRange(0, TotalWeight);
		UE_LOG(LogTemp, Warning, TEXT("RandomVal: %f"), RandomVal)
	
	
	for (const TPair<UFTAGameplayAbility*, FScoringCriteria>& Pair : ScoreMap)
	{
		UFTAGameplayAbility* Ability = Pair.Key;
		FScoringCriteria Criteria = Pair.Value;

		RandomVal -= Criteria.FinalScore;
		
		if(RandomVal <= 0.0f)
		{
			BestAbility = Ability;
			break;
		}
	}

	if(BestAbility)
	{
			UE_LOG(LogTemp, Warning, TEXT("Best Ability: %s"),
		*GetNameSafe(BestAbility));
			return BestAbility;
	}
	return nullptr;
}
