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
}
