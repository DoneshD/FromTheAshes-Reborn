#include "Input/FTAInputConfig.h"

UFTAInputConfig::UFTAInputConfig(const FObjectInitializer& ObjectInitializer)
{
	
}

const UInputAction* UFTAInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FFTAInputAction& Action : NativeInputActions)
	{
		if(Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	return nullptr;
}

const UInputAction* UFTAInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FFTAInputAction& Action : AbilityInputActions)
	{
		if(Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}
	
	UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	return nullptr;
}
