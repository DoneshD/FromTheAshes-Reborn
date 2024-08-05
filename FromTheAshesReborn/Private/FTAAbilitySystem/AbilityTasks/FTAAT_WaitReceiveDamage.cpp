
#include "FTAAbilitySystem/AbilityTasks/FTAAT_WaitReceiveDamage.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

UFTAAT_WaitReceiveDamage::UFTAAT_WaitReceiveDamage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TriggerOnce = false;
}

UFTAAT_WaitReceiveDamage* UFTAAT_WaitReceiveDamage::WaitReceiveDamage(UGameplayAbility* OwningAbility, bool InTriggerOnce)
{
	UFTAAT_WaitReceiveDamage* MyObj = NewAbilityTask<UFTAAT_WaitReceiveDamage>(OwningAbility);
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

void UFTAAT_WaitReceiveDamage::Activate()
{
	UFTAAbilitySystemComponent* FTAASC = Cast<UFTAAbilitySystemComponent>(AbilitySystemComponent);

	if (FTAASC)
	{
		FTAASC->ReceivedDamage.AddDynamic(this, &UFTAAT_WaitReceiveDamage::OnDamageReceived);
	}
}

void UFTAAT_WaitReceiveDamage::OnDestroy(bool AbilityIsEnding)
{
	UFTAAbilitySystemComponent* FTAASC = Cast<UFTAAbilitySystemComponent>(AbilitySystemComponent);

	if (FTAASC)
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