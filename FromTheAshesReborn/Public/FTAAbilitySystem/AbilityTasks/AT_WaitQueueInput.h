#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_WaitQueueInput.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UAT_WaitQueueInput : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAT_WaitQueueInput(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	static UAT_WaitQueueInput* WaitQueueInput(UGameplayAbility* OwningAbility);

	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;

	UFUNCTION()
	void OnInputReceived(class UFTAAbilitySystemComponent* SourceASC);
	
};

