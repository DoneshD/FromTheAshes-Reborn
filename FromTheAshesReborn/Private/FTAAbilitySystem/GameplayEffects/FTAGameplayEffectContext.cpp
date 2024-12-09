#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"


FFTAGameplayEffectContext* FFTAGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FFTAGameplayEffectContext::StaticStruct()))
	{
		return (FFTAGameplayEffectContext*)BaseEffectContext;
	}
	return nullptr;
}

void FFTAGameplayEffectContext::SetAbilitySource(const IFTAAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

// FTAGameplayEffectContext::FTAGameplayEffectContext()
// {
// }
//
// FTAGameplayEffectContext::~FTAGameplayEffectContext()
// {
// }
