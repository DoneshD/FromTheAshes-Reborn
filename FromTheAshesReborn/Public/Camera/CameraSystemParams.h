#pragma once

#include "CoreMinimal.h"
#include "CameraSystemParams.generated.h"


USTRUCT(BlueprintType)
struct FSpringArmLengthParams
{
	GENERATED_BODY()

	FSpringArmLengthParams()
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera | Anchor", meta=(EditCondition="ShouldAdjustAnchor"))
	float DeltaAnchorInterpSpeed = 5.0f;
	
};

USTRUCT(BlueprintType)
struct FCameraSystemParams
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	FCameraComponentParams CameraComponentParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SpringArm")
	FSpringArmLengthParams ArmLengthParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anchor")
	FCameraAnchorParams CameraAnchorParams;
	
};