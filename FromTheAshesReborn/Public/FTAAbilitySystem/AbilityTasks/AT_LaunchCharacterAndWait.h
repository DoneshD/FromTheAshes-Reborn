#pragma once
#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_LaunchCharacterAndWait.generated.h"

class AFTACharacter;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLaunchComplete);

class UCharacterMovementComponent;
class UFTAAbilitySystemComponent;

UCLASS()
class FROMTHEASHESREBORN_API UAT_LaunchCharacterAndWait : public UAbilityTask
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnLaunchComplete OnLaunchComplete;

protected:

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CMC;

	UPROPERTY()
	TObjectPtr<AFTACharacter> FTAChar;

	bool IsLaunching = false;

	FVector LaunchStartLocation;
	FVector LaunchEndLocation;
	
	float LaunchElapsedTime = 0.0f;
	float LaunchedStartTime = 0.0f;

	float LaunchVerticalDistance;
	float LaunchDuration;
	float StallDuration;
	float LaunchOffset;

	FTimerHandle AerialStallTimerHandle;


public:
	
	static UAT_LaunchCharacterAndWait* AT_LaunchCharacterAndWait(UGameplayAbility* OwningAbility, float VerticalDistance, float Duration, float StallTime, float Offset);

protected:
	
	UAT_LaunchCharacterAndWait(const FObjectInitializer& ObjectInitializer);
	
	virtual void TickTask(float DeltaTime) override;

	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;

	void UpdateMovement(float DeltaTime);
	void LocationReached();
	void EndStall();
	
};

