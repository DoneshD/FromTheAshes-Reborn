#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_WaitInputQueueAndInputWindow.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UAT_WaitInputQueueAndInputWindow : public UAbilityTask
{
	GENERATED_BODY()

public:

	UAT_WaitInputQueueAndInputWindow(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	static UAT_WaitInputQueueAndInputWindow* WaitInputQueueAndInputWindow(UGameplayAbility* OwningAbility);

	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;

	UFUNCTION()
	void OnInputQueueTagReceived(class UFTAAbilitySystemComponent* SourceASC, FGameplayTag InputTag);
};
