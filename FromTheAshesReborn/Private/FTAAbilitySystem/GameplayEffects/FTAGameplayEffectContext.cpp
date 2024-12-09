﻿#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"


FFTAGameplayEffectContext* FFTAGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FFTAGameplayEffectContext::StaticStruct()))
	{
		return (FFTAGameplayEffectContext*)BaseEffectContext;
	}
	return nullptr;
}

// FTAGameplayEffectContext::FTAGameplayEffectContext()
// {
// }
//
// FTAGameplayEffectContext::~FTAGameplayEffectContext()
// {
// }
