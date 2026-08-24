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

	TArray<UFTAGameplayAbility*> PossibleAbilities;

	

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

	auto RandomItem = PossibleAbilities[FMath::RandRange(0, PossibleAbilities.Num() - 1)];

	if(RandomItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Random Ability chosen: %s"), *RandomItem->GetName());
		InstanceData.AbilityAsset = RandomItem->DefaultAbilityDataAsset;
	}
	
}

void FStateTreeEvaluator_SelectBestAbility::TreeStop(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorCommonBase::TreeStop(Context);
}

void FStateTreeEvaluator_SelectBestAbility::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FStateTreeEvaluatorCommonBase::Tick(Context, DeltaTime);
}
