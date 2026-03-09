#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "FTAAT_MoveToLocationAndWait.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveCompleted);

class AFTACharacter;
class UCharacterMovementComponent;

UCLASS()
class FROMTHEASHESREBORN_API UFTAAT_MoveToLocationAndWait : public UAbilityTask
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnMoveCompleted OnMoveCompleted;

protected:

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CMC;

	UPROPERTY()
	TObjectPtr<AFTACharacter> FTAChar;

	bool IsMoving = false;

	FVector StartLocation;
	FVector EndLocation;
	
	float ElapsedTime = 0.0f;
	float StartTime = 0.0f;

	float LaunchVerticalDistance;
	float LaunchDuration;
	float StallDuration;
	float LaunchOffset;

	FTimerHandle AerialStallTimerHandle;


public:
	
	static UFTAAT_MoveToLocationAndWait* FTAAT_MoveToLocationAndWait(UGameplayAbility* OwningAbility);

protected:
	
	UFTAAT_MoveToLocationAndWait(const FObjectInitializer& ObjectInitializer);
	
	virtual void TickTask(float DeltaTime) override;

	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;

	void UpdateMovement(float DeltaTime);
	void LocationReached();
};