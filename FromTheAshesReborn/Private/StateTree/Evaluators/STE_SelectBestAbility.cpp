#include "StateTree/Evaluators/STE_SelectBestAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "CombatComponents/ComboManagerComponent.h"
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
			UComboManagerComponent* CMC = InstanceData.InputActor->FindComponentByClass<UComboManagerComponent>();
			if(CMC)
			{			}
			InstanceData.AbilityAsset = AbilityCDO->DefaultAbilityDataAsset;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FStateTreeEvaluator_SelectBestAbility::TreeStart - No Attacks to choose from"))
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
