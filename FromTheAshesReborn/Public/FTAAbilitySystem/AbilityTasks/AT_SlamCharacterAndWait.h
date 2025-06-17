#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_SlamCharacterAndWait.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlamComplete);

class UCharacterMovementComponent;

UCLASS()
class FROMTHEASHESREBORN_API UAT_SlamCharacterAndWait : public UAbilityTask
{
	GENERATED_BODY()

protected:
	
	TObjectPtr<UCharacterMovementComponent> CMC;
	
	bool IsSlamming = false;

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnSlamComplete OnLaunchComplete;
public:
	
	static UAT_SlamCharacterAndWait* AT_SlamCharacterAndWait(UGameplayAbility* OwningAbility);

protected:
	
	UAT_SlamCharacterAndWait(const FObjectInitializer& ObjectInitializer);
	
	virtual void TickTask(float DeltaTime) override;

	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;

	void UpdateMovement(float DeltaTime);
	void LocationReached();
};
