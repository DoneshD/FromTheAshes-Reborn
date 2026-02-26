#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CameraParamsDataAsset.generated.h"

UENUM(BlueprintType)
enum class ECameraState : uint8
{
	Free,
	LockOn
};

UENUM(BlueprintType)
enum class ECameraOperation : uint8
{
	Set,
	Additive
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base")
	bool ShouldAdjust = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base", meta=(EditCondition="ShouldAdjust"))
	ECameraOperation CameraOperation = ECameraOperation::Set;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base", meta=(EditCondition="ShouldAdjust"))
	float Priority = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lerp", meta=(EditCondition="ShouldAdjust"))
	float InLerpSpeedFloat = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lerp", meta=(EditCondition="ShouldAdjust"))
	TObjectPtr<UCurveFloat> InLerpSpeedCurve;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lerp", meta=(EditCondition="ShouldAdjust"))
	float OutLerpSpeed = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lerp", meta=(EditCondition="ShouldAdjust"))
	TObjectPtr<UCurveFloat> OutLerpSpeedCurve;
};

USTRUCT(BlueprintType)
struct FCameraFloatParam
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Value")
	float Value = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MetaData")
	FCameraValueData MetaData;

};

USTRUCT(BlueprintType)
struct FCameraVectorParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value")
	FVector Value = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaData")
	FCameraValueData MetaData;
};

USTRUCT(BlueprintType)
struct FCameraRotatorParam
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value")
	FRotator Value = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MetaData")
	FCameraValueData MetaData;

};

USTRUCT(BlueprintType)
struct FBaseCameraParams
{
	GENERATED_BODY()

public:

	FBaseCameraParams()
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base Params")
	bool InheritBase = false;

};

USTRUCT(BlueprintType)
struct FSpringArmParams : public FBaseCameraParams
{
	GENERATED_BODY()

	FSpringArmParams()
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpringArm")
	FCameraFloatParam ArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpringArm")
	FCameraVectorParam SocketOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpringArm")
	FCameraVectorParam TargetOffset;
	
};

USTRUCT(BlueprintType)
struct FCameraComponentParams : public FBaseCameraParams
{
	GENERATED_BODY()

	FCameraComponentParams()
	{}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
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
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anchor")
	FCameraVectorParam TargetLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anchor")
	FCameraRotatorParam TargetRotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anchor")
	bool UseWorldTransform = false;
	
};

USTRUCT(BlueprintType)
struct FControlRotation : public FBaseCameraParams
{
	GENERATED_BODY()

	FControlRotation()
		: TargetControlRotation(FRotator::ZeroRotator)
	{}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anchor")
	FCameraRotatorParam TargetControlRotation;
	
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

	UPROPERTY(EditAnywhere, Category = "State")
	ECameraState CameraState;
	
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
