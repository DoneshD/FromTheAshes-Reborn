#include "StateTree/Conditions/STC_CheckTargetAttackTokens.h"
#include "StateTreeExecutionContext.h"
#include "CombatComponents/GroupCombatComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "Kismet/GameplayStatics.h"

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
	// APlayerCharacter* PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if(!FTACharTarget && !FTACharTarget->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("FTACharTarget is null"))
		return false;
	}

	UGroupCombatComponent* TargetGCC = FTACharTarget->FindComponentByClass<UGroupCombatComponent>();

	if(!TargetGCC && !TargetGCC->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("TargetGCC is null"))
		return false;
	}

	// 	UE_LOG(LogTemp, Error,
	// 	TEXT("Owner: %s | AmountToCheck: %d | Target: %s | AttackTokensCount: %d"),
	// 	*InstanceData.Owner->GetName(),
	// 	InstanceData.AmountToCheck,
	// 	*InstanceData.Target->GetName(),
	// 	TargetGCC->AttackTokensCount
	// );
	
	// TargetGCC->AttackTokensCount = 1;
	if (TargetGCC->AttackTokensCount < InstanceData.AmountToCheck)
	{
		// UE_LOG(
		// 	LogTemp,
		// 	Warning,
		// 	TEXT("Attack token check FAILED: AttackTokensCount (%d) < AmountToCheck (%d)"),
		// 	TargetGCC->AttackTokensCount,
		// 	InstanceData.AmountToCheck
		// );

		return false;
	}

	// UE_LOG(
	// 	LogTemp,
	// 	Warning,
	// 	TEXT("Attack token check SUCCEEDED: AttackTokensCount (%d) >= AmountToCheck (%d)"),
	// 	TargetGCC->AttackTokensCount,
	// 	InstanceData.AmountToCheck
	// );

	return true;
}
