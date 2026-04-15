#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"
#include "AT_WaitInputTagAndQueueWindowEvent.generated.h"


USTRUCT()
struct FQueuedInputData
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag InputTag;

	UPROPERTY()
	ELockOnInputOrientationDirection Direction;
	
};

class UFTAGameplayAbility;
class UFTAAbilitySystemComponent;

UCLASS()
class FROMTHEASHESREBORN_API UAT_WaitInputTagAndQueueWindowEvent : public UAbilityTask
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	UFTAAbilitySystemComponent* FTAASC = nullptr;
	
	FQueuedInputData QueuedInputData;

	struct FQueueWindowHandle
	{
		FDelegateHandle DelegateHandle;
	};
	
	TMap<FGameplayTag, FQueueWindowHandle> QueueWindowHandles;
	TMap<FGameplayTag, TArray<UFTAGameplayAbility*>> QueueableAbilities;

public:
	static UAT_WaitInputTagAndQueueWindowEvent* WaitInputTagAndQueueWindowEvent(UGameplayAbility* OwningAbility);

protected:
	UAT_WaitInputTagAndQueueWindowEvent(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;

	UFUNCTION()
	void OnInputTagReceived(FGameplayTag InputTag);
	
	void RegisterQueueWindowTagEvent(FGameplayTag QueueWindowTag);
	void RemoveQueueWindowTagEvent(FGameplayTag QueueWindowTag);
	void OnQueueWindowTagChanged(const FGameplayTag QueueWindowTag, int32 NewCount);
	
};
