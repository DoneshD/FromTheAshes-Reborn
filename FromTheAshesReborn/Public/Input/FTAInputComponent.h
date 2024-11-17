#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "FTAInputConfig.h"
#include "FTAInputComponent.generated.h"


struct FGameplayTag;

UCLASS(Config = Input)
class FROMTHEASHESREBORN_API UFTAInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UFTAInputComponent(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Serves as a way to pass any object that can handle input action binding, as well as any function type
	template<class UserClass, typename FuncType>
	void BindNativeAction(const UFTAInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityAction(const UFTAInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	//Maybe add a function for removing binds?
	//void RemoveBinds(TArray<uint32>& BindHandles);
	
	
};
