#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_WaitInputQueueAndInputWindow.generated.h"

class UFTAAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRegisterInputWindowTagEventSignature, FGameplayTag, InputWindowTag);

UCLASS()
class FROMTHEASHESREBORN_API UAT_WaitInputQueueAndInputWindow : public UAbilityTask
{
	GENERATED_BODY()

public:

	UFTAAbilitySystemComponent* FTAASC;

	FRegisterInputWindowTagEventSignature OnRegisterInputWindowTagEventDelegate;

	FGameplayTag CurrentInputTag;
	
	TMap<FGameplayTag, FDelegateHandle> TagDelegateHandles;
	TMap<FGameplayTag, FTimerHandle> TagTimerHandles;

	UAT_WaitInputQueueAndInputWindow(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	static UAT_WaitInputQueueAndInputWindow* WaitInputQueueAndInputWindow(UGameplayAbility* OwningAbility);
	

	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;

	UFUNCTION()
	void OnInputQueueTagReceived(class UFTAAbilitySystemComponent* SourceASC, FGameplayTag InputTag);
	
	void ChangeActivationGroupToReplaceable(FGameplayTag InputWindowTag);
	void ChangeActivationGroupToBlocking(FGameplayTag InputWindowTag);

	UFUNCTION()
	void RegisterInputWindowTagEvent(FGameplayTag InputWindowTag);
	
	virtual void InputWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	void InputWindowOpen(FGameplayTag InputWindowTag);
	
	void ProceedToNextAbility();
	
	void RemoveInputWindowTagEvent(FGameplayTag InputWindowTag);
	
};
