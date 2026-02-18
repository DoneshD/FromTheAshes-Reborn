#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CameraParamsDataAsset.generated.h"

UENUM(BlueprintType)
enum class ECameraOperation : uint8
{
	Set,
	Additive,
	LockOn,
	Override
};

UENUM(BlueprintType)
enum class ETransformType : uint8
{
	World,
	Relative
};

USTRUCT(BlueprintType)
struct FBaseCameraParams
{
	GENERATED_BODY()

public:

	FBaseCameraParams()
	{}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
	bool ShouldAdjust = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base", meta=(EditCondition="ShouldAdjust"))
	ECameraOperation CameraOperation = ECameraOperation::Set;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base", meta=(EditCondition="ShouldAdjust"))
	float Priority = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base", meta=(EditCondition="ShouldAdjust"))
	float InLerpSpeedFloat = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base", meta=(EditCondition="ShouldAdjust"))
	TObjectPtr<UCurveFloat> InLerpSpeedCurve;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base", meta=(EditCondition="ShouldAdjust"))
	float OutLerpSpeed = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base", meta=(EditCondition="ShouldAdjust"))
	TObjectPtr<UCurveFloat> OutLerpSpeedCurve;

};

USTRUCT(BlueprintType)
struct FSpringArmParams : public FBaseCameraParams
{
	GENERATED_BODY()

	FSpringArmParams()
	{}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spring Arm", meta=(EditCondition="ShouldAdjust"))
	float Value = 0;
	
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
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FOV")
	bool ShouldAdjustFOV = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FOV", meta=(EditCondition="ShouldAdjustFOV"))
	bool ShouldOverrideFOV = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FOV", meta=(EditCondition="ShouldAdjustFOV"))
	bool ShouldResetFOVOffset = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FOV", meta=(EditCondition="ShouldAdjustFOV"))
	float DeltaFOV = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FOV", meta=(EditCondition="ShouldAdjustFOV"))
	float DeltaFOVInterpSpeed = 5.0f;
	
};

USTRUCT(BlueprintType)
struct FCameraAnchorParams : public FBaseCameraParams
{
	GENERATED_BODY()

	FCameraAnchorParams()
		: TargetLocation(FVector::ZeroVector)
		, TargetRotation(FRotator::ZeroRotator)
	{}
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anchor", meta=(EditCondition="ShouldAdjust"))
	FVector TargetLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anchor", meta=(EditCondition="ShouldAdjust"))
	FRotator TargetRotation;
	
	
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

	UPROPERTY(EditAnywhere, Category = "Core Component")
	FCameraAnchorParams CameraAnchorParams;

	UPROPERTY(EditAnywhere, Category = "Core Component")
	FSpringArmParams SpringArmParams;

	UPROPERTY(EditAnywhere, Category = "Core Component")
	FCameraComponentParams CameraComponentParams;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	FInputOffsetStruct InputOffsetInfo;
	
	UPROPERTY(EditAnywhere, Category = "Catch up")
	float CatchupInterpSpeed = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance Based Offset")
	float DistanceBasedMaxPitchOffset = -20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance Based Offset")
	float DistanceBasedMaxYawOffset = -35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Rotation")
	bool ShouldUpdateControllerRotation = false;
};
