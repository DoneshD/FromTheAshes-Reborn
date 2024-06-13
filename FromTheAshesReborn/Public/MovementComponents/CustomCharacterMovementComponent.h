
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Jump Curve"))
	UCurveFloat* FJumpCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Fall Curve"))
	UCurveFloat* F_FallCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Leap Distance Curve"))
	UCurveFloat* FLeapCurveDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Leap Height Curve"))
	UCurveFloat* FLeapCurveHeight;

	//--------Jump---------

	bool IsJumping;

	float JumpTime;
	float PrevJumpCurveValue;

	int JumpCount = 0;

	float JumpMinTime;
	float JumpMaxTime;

	//--------Fall---------

	bool bIsFalling;

	float FallTime;
	float PrevFallCurveValue;

	//--------Leap---------

	bool IsLeaping;

	float LeapTime;
	float PrevLeapCurveValue;

	int LeapCount = 0;

	float LeapMinTime;
	float LeapMaxTime;

public:
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* TickFunction) override;

	virtual bool DoJump(bool bReplayingMoves) override;

	virtual bool IsFalling() const override;

	void SetCustomFallingMode();

	void ProcessCustomJump(float DeltaTime);
	void ProcessCustomFalling(float DeltaTime);
	void ProcessCustomLeap(float DeltaTime);
	bool CustomFindFloor(FFindFloorResult& OutFloorResult, const FVector StartLocation, const FVector EndLocation);
	
};

