#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CameraParamsDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FSpringArmParams
{
	GENERATED_BODY()

	FSpringArmParams()
	: ShouldAdjustArmLength(true)
	, ShouldOverrideArmLength(false)
	, ShouldResetOffset(false)
	, DeltaArmLength(0.0f)
	, DeltaArmLengthInterpSpeed(5.0f)
	{}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | Arm Length")
	bool ShouldAdjustArmLength = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | Arm Length", meta=(EditCondition="ShouldAdjustArmLength"))
	bool ShouldOverrideArmLength = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spring Arm")
	float TargetArmLength = 250.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | Arm Length", meta=(EditCondition="ShouldAdjustArmLength"))
	bool ShouldResetOffset = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | Arm Length", meta=(EditCondition="ShouldAdjustArmLength"))
	float DeltaArmLength = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | Arm Length", meta=(EditCondition="ShouldAdjustArmLength"))
	float DeltaArmLengthInterpSpeed = 5.0f;
	
};

USTRUCT(BlueprintType)
struct FCameraComponentParams
{
	GENERATED_BODY()

	FCameraComponentParams()
	: ShouldAdjustFOV(false)
	, ShouldOverrideFOV(false)
	, ShouldResetFOVOffset(false)
	, DeltaFOV(0.0f)
	, DeltaFOVInterpSpeed(5.0f)
	{}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | FOV")
	bool ShouldAdjustFOV = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | FOV", meta=(EditCondition="ShouldAdjustFOV"))
	bool ShouldOverrideFOV = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | FOV", meta=(EditCondition="ShouldAdjustFOV"))
	bool ShouldResetFOVOffset = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | FOV", meta=(EditCondition="ShouldAdjustFOV"))
	float DeltaFOV = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | FOV", meta=(EditCondition="ShouldAdjustFOV"))
	float DeltaFOVInterpSpeed = 5.0f;
	
};

USTRUCT(BlueprintType)
struct FCameraAnchorParams
{
	GENERATED_BODY()

	FCameraAnchorParams()
		: ShouldAdjustAnchor(false)
		, ShouldOverrideAnchor(false)
		, ShouldResetAnchorOffset(false)
		, ShouldUseWorldTransform(false)
		, NewAnchorLocation(FVector::ZeroVector)
		, NewAnchorRotation(FRotator::ZeroRotator)
		, DeltaAnchorInterpSpeed(5.0f)
	{}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera| FOV")
	bool ShouldAdjustAnchor = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | Anchor", meta=(EditCondition="ShouldAdjustAnchor"))
	bool ShouldOverrideAnchor = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | Anchor", meta=(EditCondition="ShouldAdjustAnchor"))
	bool ShouldResetAnchorOffset = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | Anchor", meta=(EditCondition="ShouldAdjustAnchor"))
	bool ShouldUseWorldTransform = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | Anchor", meta=(EditCondition="ShouldAdjustAnchor"))
	FVector NewAnchorLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | Anchor", meta=(EditCondition="ShouldAdjustAnchor"))
	FRotator NewAnchorRotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | Anchor")
	FTransform TargetTransform;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | Anchor", meta=(EditCondition="ShouldAdjustAnchor"))
	float DeltaAnchorInterpSpeed = 5.0f;
	
};

USTRUCT(BlueprintType)
struct FInputOffsetStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Input Offset")
	bool EnableInputBasedOffset = true;

	UPROPERTY(EditAnywhere, Category = "Input Offset")
	float InputOffsetScale = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Input Offset")
	float InputBasedMaxYawOffset = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Input Offset")
	float InputBasedMaxPitchOffset = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Input Offset")
	float InputOffsetDecayRate = 0.75f;
	
};

UCLASS()
class FROMTHEASHESREBORN_API UCameraParamsDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Input Offset")
	FInputOffsetStruct InputOffsetInfo;

	UPROPERTY(EditAnywhere, Category = "Input Offset")
	FSpringArmParams SpringArmParams;

	UPROPERTY(EditAnywhere, Category = "Input Offset")
	FCameraComponentParams CameraComponentParams;

	UPROPERTY(EditAnywhere, Category = "Input Offset")
	FCameraAnchorParams CameraAnchorParams;

	UPROPERTY(EditAnywhere, Category = "Catch up")
	float CatchupInterpSpeed = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance Based Offset")
	float DistanceBasedMaxPitchOffset = -20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance Based Offset")
	float DistanceBasedMaxYawOffset = -35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Rotation")
	bool ShouldUpdateControllerRotation = false;
};
