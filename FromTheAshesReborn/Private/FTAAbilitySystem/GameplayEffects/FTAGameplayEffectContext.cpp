#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"


FFTAGameplayEffectContext* FFTAGameplayEffectContext::ExtractEffectContext(FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();

	if (!BaseEffectContext)
	{
		UE_LOG(LogTemp, Error, TEXT("ExtractEffectContext: BaseEffectContext is NULL")
		);

		return nullptr;
	}

	if (BaseEffectContext->GetScriptStruct() != FFTAGameplayEffectContext::StaticStruct())
	{
		UE_LOG(LogTemp, Error, TEXT("ExtractEffectContext: Context is NOT FFTAGameplayEffectContext. Actual type: %s"), *BaseEffectContext->GetScriptStruct()->GetName());
		return nullptr;
	}

	return static_cast<FFTAGameplayEffectContext*>(BaseEffectContext);
}

FGameplayEffectContext* FFTAGameplayEffectContext::Duplicate() const
{
	FFTAGameplayEffectContext* NewContext = new FFTAGameplayEffectContext();

	*NewContext = *this;

	return NewContext;
}

void FFTAGameplayEffectContext::SetAbilitySource(const UObject* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
}

const IFTAAbilitySourceInterface* FFTAGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IFTAAbilitySourceInterface>(AbilitySourceObject.Get());
}
