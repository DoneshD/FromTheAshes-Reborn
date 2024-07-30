// Fill out your copyright notice in the Description page of Project Settings.


#include "FTAAbilitySystem/FTAAbilitySystemGlobals.h"
#include "FTAAbilitySystem/FTAGameplayEffectTypes.h"

UFTAAbilitySystemGlobals::UFTAAbilitySystemGlobals()
{

}

// FGameplayEffectContext* UFTAAbilitySystemGlobals::AllocGameplayEffectContext() const
// {
// 	return new FFTAGameplayEffectContext();
// }

void UFTAAbilitySystemGlobals::InitGlobalTags()
{
	Super::InitGlobalTags();

	DeadTag = FGameplayTag::RequestGameplayTag("State.Dead");
	KnockedDownTag = FGameplayTag::RequestGameplayTag("State.KnockedDown");
	InteractingTag = FGameplayTag::RequestGameplayTag("State.Interacting");
	InteractingRemovalTag = FGameplayTag::RequestGameplayTag("State.InteractingRemoval");
}
