#include "StateTree/Tasks/STT_MeleeWeaponAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "StateTreeExecutionContext.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"


class UAbilitySystemComponent;

EStateTreeRunStatus FStateTreeTask_MeleeWeaponAttack::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return ActivateMeleeAttack(Context);
}

EStateTreeRunStatus FStateTreeTask_MeleeWeaponAttack::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return FStateTreeTaskCommonBase::Tick(Context, DeltaTime);
}

void FStateTreeTask_MeleeWeaponAttack::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	UE_LOG(LogTemp, Warning, TEXT("EXIT"))
	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}

EStateTreeRunStatus FStateTreeTask_MeleeWeaponAttack::ActivateMeleeAttack(const FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.InputActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Input Actor is null"))
		return EStateTreeRunStatus::Failed;
	}
	UE_LOG(LogTemp, Warning, TEXT("Actor name: %s"), *InstanceData.InputActor->GetName());
	
	FGameplayEventData EventData;
	EventData.Instigator = InstanceData.InputActor;
	EventData.EventTag = FGameplayTag::RequestGameplayTag("TestTag.Tag9");

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InstanceData.InputActor, EventData.EventTag, EventData);
	
	return EStateTreeRunStatus::Running;
}
