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
	FVector SlamLocation;
	float SlamSpeed;
	float SlamDuration;

	float SlamDownwardDistance;

	float SlamElapsedTime = 0.0f;
	float SlamStartTime = 0.0f;

	FVector SlamStartLocation;
	FVector SlamEndLocation;

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnSlamComplete OnSlamComplete;
	
public:
	
	static UAT_SlamCharacterAndWait* AT_SlamCharacterAndWait(UGameplayAbility* OwningAbility, FVector SlamLocation, float Speed, float Duration);

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
