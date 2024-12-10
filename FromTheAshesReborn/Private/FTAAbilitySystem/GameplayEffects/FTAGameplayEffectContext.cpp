#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"


FFTAGameplayEffectContext* FFTAGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	
	if((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct())
	{
		return (FFTAGameplayEffectContext*)BaseEffectContext;
	}
	return nullptr;
}

void FFTAGameplayEffectContext::SetAbilitySource(const IFTAAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
}

const IFTAAbilitySourceInterface* FFTAGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IFTAAbilitySourceInterface>(AbilitySourceObject.Get());
}
