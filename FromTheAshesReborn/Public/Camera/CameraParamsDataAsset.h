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
struct FCameraValueData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lerp")
	ECameraOperation Operation = ECameraOperation::Set;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lerp")
	float InLerpSpeedFloat = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lerp")
	TObjectPtr<UCurveFloat> InLerpSpeedCurve;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lerp")
	float OutLerpSpeed = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lerp")
	TObjectPtr<UCurveFloat> OutLerpSpeedCurve;
};

USTRUCT(BlueprintType)
struct FCameraFloatParam
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Float")
	float Value = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lerp")
	FCameraValueData MetaData;

};

USTRUCT(BlueprintType)
struct FCameraVectorParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value")
	FVector Value = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lerp")
	FCameraValueData MetaData;
};

USTRUCT(BlueprintType)
struct FCameraRotatorParam
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value")
	FRotator Value = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lerp")
	FCameraValueData MetaData;

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

};

USTRUCT(BlueprintType)
struct FSpringArmParams : public FBaseCameraParams
{
	GENERATED_BODY()

	FSpringArmParams()
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpringArm", meta=(EditCondition="ShouldAdjust"))
	FCameraFloatParam ArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpringArm", meta=(EditCondition="ShouldAdjust"))
	FCameraVectorParam SocketOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpringArm", meta=(EditCondition="ShouldAdjust"))
	FCameraVectorParam TargetOffset;
	
};

USTRUCT(BlueprintType)
struct FCameraComponentParams : public FBaseCameraParams
{
	GENERATED_BODY()

	FCameraComponentParams()
	{}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spring Arm", meta=(EditCondition="ShouldAdjust"))
	float FOV = 90.0f;
	
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
	FCameraVectorParam TargetLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anchor", meta=(EditCondition="ShouldAdjust"))
	FCameraRotatorParam TargetRotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anchor", meta=(EditCondition="ShouldAdjust"))
	bool UseWorldTransform = false;
	
};

USTRUCT(BlueprintType)
struct FControlRotation : public FBaseCameraParams
{
	GENERATED_BODY()

	FControlRotation()
		: TargetControlRotation(FRotator::ZeroRotator)
	{}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anchor", meta=(EditCondition="ShouldAdjust"))
	FRotator TargetControlRotation;
	
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

	UPROPERTY(EditAnywhere, Category = "Core Component")
	FControlRotation ControlRotationParams;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	FInputOffsetStruct InputOffsetInfo;
	
};
