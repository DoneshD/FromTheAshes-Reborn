#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_SuspendInAirAndWait.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSuspendComplete);

class UCharacterMovementComponent;

UCLASS()
class FROMTHEASHESREBORN_API UAT_SuspendInAirAndWait : public UAbilityTask
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnSuspendComplete OnSuspendComplete;

protected:
	
	TObjectPtr<UCharacterMovementComponent> CMC;
	
	bool bDescend = false; 
	float DescentSpeed;
	
	float SuspendDuration;
	
	FTimerHandle AerialSuspensionTimer;

public:
	
	static UAT_SuspendInAirAndWait* AT_SuspendInAirAndWait(UGameplayAbility* OwningAbility, float Speed, float Duration);

protected:
	
	UAT_SuspendInAirAndWait(const FObjectInitializer& ObjectInitializer);
	virtual void TickTask(float DeltaTime) override;
	virtual void Activate() override;

	void UpdateDescentMovement(float DeltaTime);
	void EndStall();
	
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;
	
};


