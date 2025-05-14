
#include "FTAAbilitySystem/AbilityTasks/FTAAT_WaitReceiveDamage.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

UFTAAT_WaitReceiveDamage::UFTAAT_WaitReceiveDamage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TriggerOnce = false;
}

UFTAAT_WaitReceiveDamage* UFTAAT_WaitReceiveDamage::WaitReceiveDamage(UGameplayAbility* OwningAbility, bool InTriggerOnce)
{
	UFTAAT_WaitReceiveDamage* WaitDamageTask = NewAbilityTask<UFTAAT_WaitReceiveDamage>(OwningAbility);
	WaitDamageTask->TriggerOnce = InTriggerOnce;
	
	return WaitDamageTask;
}

void UFTAAT_WaitReceiveDamage::Activate()
{
	if (UFTAAbilitySystemComponent* FTAASC = Cast<UFTAAbilitySystemComponent>(AbilitySystemComponent))
	{
		FTAASC->ReceivedDamage.AddDynamic(this, &UFTAAT_WaitReceiveDamage::OnDamageReceived);
	}
}

void UFTAAT_WaitReceiveDamage::OnDestroy(bool AbilityIsEnding)
{
	if (UFTAAbilitySystemComponent* FTAASC = Cast<UFTAAbilitySystemComponent>(AbilitySystemComponent))
	{
		FTAASC->ReceivedDamage.RemoveDynamic(this, &UFTAAT_WaitReceiveDamage::OnDamageReceived);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UFTAAT_WaitReceiveDamage::OnDamageReceived(UFTAAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
	}

	if (TriggerOnce)
	{
		EndTask();
	}
}