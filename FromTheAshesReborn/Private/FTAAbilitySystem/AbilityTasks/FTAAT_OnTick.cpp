#include "FTAAbilitySystem/AbilityTasks/FTAAT_OnTick.h"

UFTAAT_OnTick::UFTAAT_OnTick(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}


void UFTAAT_OnTick::Activate()
{
	Super::Activate();
}

void UFTAAT_OnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTick.Broadcast(DeltaTime);
	}
}

UFTAAT_OnTick* UFTAAT_OnTick::StartTicking(UGameplayAbility* OwningAbility)
{
	UFTAAT_OnTick* MyTask = NewAbilityTask<UFTAAT_OnTick>(OwningAbility);
	return MyTask;
}

FString UFTAAT_OnTick::GetDebugString() const
{
	return TEXT("TickTask: UFTAAT_Tick running");
}

void UFTAAT_OnTick::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);
}
