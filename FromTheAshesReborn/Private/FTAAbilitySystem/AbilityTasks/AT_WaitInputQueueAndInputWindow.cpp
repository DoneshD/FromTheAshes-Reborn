#include "FTAAbilitySystem/AbilityTasks/AT_WaitInputQueueAndInputWindow.h"

#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

UAT_WaitInputQueueAndInputWindow::UAT_WaitInputQueueAndInputWindow(const FObjectInitializer& ObjectInitializer)
{
	
}

UAT_WaitInputQueueAndInputWindow* UAT_WaitInputQueueAndInputWindow::WaitInputQueueAndInputWindow(UGameplayAbility* OwningAbility)
{
	UAT_WaitInputQueueAndInputWindow* Obj = NewAbilityTask<UAT_WaitInputQueueAndInputWindow>(OwningAbility);
	
	return Obj;
}

void UAT_WaitInputQueueAndInputWindow::Activate()
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
		FTAASC->OnQueueInputReceived.AddDynamic(this, &UAT_WaitInputQueueAndInputWindow::OnInputQueueTagReceived);
	}
}


void UAT_WaitInputQueueAndInputWindow::OnInputQueueTagReceived(UFTAAbilitySystemComponent* SourceASC,FGameplayTag InputTag)
{
	UE_LOG(LogTemp, Warning, TEXT("OnInputReceived"))
	UE_LOG(LogTemp, Warning, TEXT("Tag Name: %s"), *InputTag.GetTagName().ToString());
}

void UAT_WaitInputQueueAndInputWindow::ExternalCancel()
{
	Super::ExternalCancel();
}

FString UAT_WaitInputQueueAndInputWindow::GetDebugString() const
{
	return Super::GetDebugString();
}

void UAT_WaitInputQueueAndInputWindow::OnDestroy(bool AbilityEnded)
{

	UFTAAbilitySystemComponent* FTAASC = Cast<UFTAAbilitySystemComponent>(AbilitySystemComponent);

	if (FTAASC)
	{
		FTAASC->OnQueueInputReceived.RemoveDynamic(this, &UAT_WaitInputQueueAndInputWindow::OnInputQueueTagReceived);
	}

	Super::OnDestroy(AbilityEnded);

}

