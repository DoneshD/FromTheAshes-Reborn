#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "FTAInputConfig.h"
#include "FTAInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UObject;
struct FGameplayTag;

UCLASS(Config = Input)
class FROMTHEASHESREBORN_API UFTAInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	
	UFTAInputComponent(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UFTAInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityAction(const UFTAInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);
	
};


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
					BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
				}
			}
		}
	}
	
	
