#include "FTAAbilitySystem/AbilityTasks/AT_WaitQueueInput.h"

#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

UAT_WaitQueueInput::UAT_WaitQueueInput(const FObjectInitializer& ObjectInitializer)
{
	
}

UAT_WaitQueueInput* UAT_WaitQueueInput::WaitQueueInput(UGameplayAbility* OwningAbility)
{
	UAT_WaitQueueInput* Obj = NewAbilityTask<UAT_WaitQueueInput>(OwningAbility);
	
	return Obj;
}

void UAT_WaitQueueInput::Activate()
{
	Super::Activate();
	if (!Ability)
	{
		UE_LOG(LogTemp, Error, TEXT("UAT_WaitQueueInput - Ability is invalid"))
		return;
	}
	UFTAAbilitySystemComponent* FTAASC = Cast<UFTAAbilitySystemComponent>(AbilitySystemComponent);

	if (FTAASC)
	{
		FTAASC->OnQueueInputReceived.AddDynamic(this, &UAT_WaitQueueInput::OnInputReceived);
	}
}

void UAT_WaitQueueInput::OnInputReceived(UFTAAbilitySystemComponent* SourceASC)
{
	UE_LOG(LogTemp, Warning, TEXT("OnInputReceived"))

}

void UAT_WaitQueueInput::ExternalCancel()
{
	Super::ExternalCancel();
}

FString UAT_WaitQueueInput::GetDebugString() const
{
	return Super::GetDebugString();
}

void UAT_WaitQueueInput::OnDestroy(bool AbilityEnded)
{

	UFTAAbilitySystemComponent* FTAASC = Cast<UFTAAbilitySystemComponent>(AbilitySystemComponent);

	if (FTAASC)
	{
		FTAASC->OnQueueInputReceived.RemoveDynamic(this, &UAT_WaitQueueInput::OnInputReceived);
	}

	Super::OnDestroy(AbilityEnded);

}
