#include "StateTree/Evaluators/STE_SelectBestAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_Attack.h"

void FStateTreeEvaluator_SelectBestAbility::TreeStart(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorCommonBase::TreeStart(Context);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.AICombatParams->PossibleAttacks.Num() > 0)
	{
		auto RandomItem = InstanceData.AICombatParams->PossibleAttacks[FMath::RandRange(0, InstanceData.AICombatParams->PossibleAttacks.Num() - 1)];
		UGA_Attack* AbilityCDO = RandomItem->GetDefaultObject<UGA_Attack>();
		if(AbilityCDO)
		{
			UE_LOG(LogTemp, Warning, TEXT("Asset found: %s"), *AbilityCDO->DefaultAbilityDataAsset->GetName());
			InstanceData.AbilityAsset = AbilityCDO->DefaultAbilityDataAsset;
		}
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
