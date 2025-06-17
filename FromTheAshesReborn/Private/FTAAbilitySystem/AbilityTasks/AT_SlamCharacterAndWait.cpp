#include "FTAAbilitySystem/AbilityTasks/AT_SlamCharacterAndWait.h"

UAT_SlamCharacterAndWait* UAT_SlamCharacterAndWait::AT_SlamCharacterAndWait(UGameplayAbility* OwningAbility)
{
	UAT_SlamCharacterAndWait* Task = NewAbilityTask<UAT_SlamCharacterAndWait>(OwningAbility);
	return Task;

}

UAT_SlamCharacterAndWait::UAT_SlamCharacterAndWait(const FObjectInitializer& ObjectInitializer)
{
}

void UAT_SlamCharacterAndWait::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
}

void UAT_SlamCharacterAndWait::Activate()
{
	Super::Activate();
}

void UAT_SlamCharacterAndWait::ExternalCancel()
{
	Super::ExternalCancel();
}

FString UAT_SlamCharacterAndWait::GetDebugString() const
{
	return Super::GetDebugString();
}

void UAT_SlamCharacterAndWait::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);
}

void UAT_SlamCharacterAndWait::UpdateMovement(float DeltaTime)
{
}

void UAT_SlamCharacterAndWait::LocationReached()
{
}
