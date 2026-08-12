#include "StateTree/Tasks/STT_SelectBestAbilityFromList.h"

#include "StateTreeExecutionContext.h"
#include "DataAsset/FTAAbilityDataAsset.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"

EStateTreeRunStatus FStateTreeTask_SelectBestAbilityFromList::EnterState(FStateTreeExecutionContext& Context,
                                                                         const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskCommonBase::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.PossibleAbilities.Num() > 0)
	{
		auto RandomItem = InstanceData.PossibleAbilities[FMath::RandRange(0, InstanceData.PossibleAbilities.Num() - 1)];
		UFTAGameplayAbility* AbilityCDO = RandomItem->GetDefaultObject<UFTAGameplayAbility>();
		if(AbilityCDO)
		{
			UE_LOG(LogTemp, Warning, TEXT("Asset found: %s"), *AbilityCDO->DefaultAbilityDataAsset->GetName());
			InstanceData.SelectedAbilityAsset = AbilityCDO->DefaultAbilityDataAsset;
			return EStateTreeRunStatus::Succeeded;
		}
	}
	return EStateTreeRunStatus::Failed;
	
}

EStateTreeRunStatus FStateTreeTask_SelectBestAbilityFromList::Tick(FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	return FStateTreeTaskCommonBase::Tick(Context, DeltaTime);
}

void FStateTreeTask_SelectBestAbilityFromList::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}
