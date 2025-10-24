#include "StateTree/Conditions/STC_CheckTargetAttackTokens.h"
#include "StateTreeExecutionContext.h"
#include "CombatComponents/GroupCombatComponent.h"
#include "FTACustomBase/FTACharacter.h"

bool FStateTreeCondition_CheckTargetAttackTokens::TestCondition(FStateTreeExecutionContext& Context) const
{
	// FStateTreeConditionCommonBase::TestCondition(Context);

	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if(!InstanceData.Owner && !InstanceData.Owner->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is null"))
		return false;
	}

	if(!InstanceData.Target && !InstanceData.Target->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("Target is null"))
		return false;

	}

	AFTACharacter* FTACharTarget = Cast<AFTACharacter>(InstanceData.Target);

	if(!FTACharTarget && !FTACharTarget->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("FTACharTarget is null"))
		return false;
	}

	UGroupCombatComponent* GroupCombatComponent = FTACharTarget->FindComponentByClass<UGroupCombatComponent>();

	if(!GroupCombatComponent && !GroupCombatComponent->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("GroupCombatComponent is null"))
		return false;
	}

	if(GroupCombatComponent->AttackTokensCount < InstanceData.AmountToCheck)
	{
		return false;
	}
	return true;
}
