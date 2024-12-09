#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"


FFTAGameplayEffectContext* FFTAGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();

	if(!BaseEffectContext)
	{
		UE_LOG(LogTemp, Error, TEXT("WTF"));
	}

	if(!BaseEffectContext->GetScriptStruct())
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR"));
	}
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

// FTAGameplayEffectContext::FTAGameplayEffectContext()
// {
// }
//
// FTAGameplayEffectContext::~FTAGameplayEffectContext()
// {
// }
