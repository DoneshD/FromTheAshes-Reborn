#include "Input/FTAInputComponent.h"

UFTAInputComponent::UFTAInputComponent(const FObjectInitializer& ObjectInitializer)
{
	
}

void UFTAInputComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UFTAInputComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


template <class UserClass, typename FuncType>
void UFTAInputComponent::BindNativeAction(const UFTAInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
{
	check(InputConfig);
	if(const UInputAction* InputAction = InputConfig->FindNativeInputActionForTag(InputTag))
	{
		BindAction(InputAction, TriggerEvent, Object, Func);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UFTAInputComponent::BindAbilityAction(const UFTAInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	//Maybe use UFTAInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag) const;

	for (const FFTAInputAction Action : InputConfig->AbilityInputActions)
	{
		if(Action.InputAction && Action.InputTag.IsValid())
		{
			if(PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}
			if(ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, PressedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}