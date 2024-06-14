
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Current Height Curve"))
	UCurveFloat* FCurrentHeightCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Current Distance Curve"))
	UCurveFloat* FCurrentDistanceCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Spring Height Curve"))
	UCurveFloat* FSpringHeightCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Spring Distance Curve"))
	UCurveFloat* FSpringDistanceCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Leap Height Curve"))
	UCurveFloat* FLeapHeightCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Leap Distance Curve"))
	UCurveFloat* FLeapDistanceCurve;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Fall Curve"))
	UCurveFloat* F_FallCurve;

	//--------Current---------

	float CurrentTime;
	float PrevCurrentCurveValue;

	float CurrentMinTime;
	float CurrentMaxTime;

	//--------Jump---------

	bool IsJumping;

	int JumpCount = 0;

	//--------Spring---------

	bool IsSpringing;

	float SpringTime;
	float PrevSpringCurveValue;

	int SpringCount = 0;

	float SpringMinTime;
	float SpringMaxTime;

	//--------Leap---------

	bool IsLeaping;

	float LeapTime;
	float PrevLeapCurveValue;

	float LeapMinTime;
	float LeapMaxTime;

	//--------Fall---------

	bool bIsFalling;

	float FallTime;
	float PrevFallCurveValue;

public:
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* TickFunction) override;

	virtual bool DoJump(bool bReplayingMoves) override;

	virtual bool IsFalling() const override;

	void SetCustomFallingMode();

	void ProcessCustomJump(float DeltaTime);
	void ProcessCustomFalling(float DeltaTime);
	
	bool CustomFindFloor(FFindFloorResult& OutFloorResult, const FVector StartLocation, const FVector EndLocation);
	
};

