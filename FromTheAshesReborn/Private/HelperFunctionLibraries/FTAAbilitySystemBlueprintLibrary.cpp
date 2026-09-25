#include "HelperFunctionLibraries/FTAAbilitySystemBlueprintLibrary.h"

#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"

UFTACueObject* UFTAAbilitySystemBlueprintLibrary::GetCueObjectFromEffectContext(
	const FGameplayEffectContextHandle& Context)
{
	FFTAGameplayEffectContext* FTAContext =
		FFTAGameplayEffectContext::ExtractEffectContext(Context);

	if (!FTAContext)
	{
		return nullptr;
	}

	return FTAContext->CueObject;
}
