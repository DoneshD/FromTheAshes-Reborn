#pragma once

#include "CoreMinimal.h"
#include "CameraSystemParams.generated.h"


USTRUCT(BlueprintType)
struct FSpringArmLengthParams
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Arm Length")
	bool ShouldAdjustArmLength = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Arm Length", meta=(EditCondition="ShouldAdjustArmLength"))
	bool ShouldOverrideArmLength = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Arm Length", meta=(EditCondition="ShouldAdjustArmLength"))
	bool ShouldResetOffset = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Arm Length", meta=(EditCondition="ShouldAdjustArmLength"))
	float DeltaArmLength = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Arm Length", meta=(EditCondition="ShouldAdjustArmLength"))
	float DeltaArmLengthInterpSpeed = 5.0f;
	
};

USTRUCT(BlueprintType)
struct FCameraComponentParams
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera| FOV")
	bool ShouldAdjustFOV = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Arm Length", meta=(EditCondition="ShouldAdjustFOV"))
	bool ShouldOverrideFOV = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Arm Length", meta=(EditCondition="ShouldAdjustFOV"))
	bool ShouldResetFOVOffset = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Arm Length", meta=(EditCondition="ShouldAdjustFOV"))
	float DeltaFOV = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Arm Length", meta=(EditCondition="ShouldAdjustFOV"))
	float DeltaFOVInterpSpeed = 5.0f;
	
};

USTRUCT(BlueprintType)
struct FCameraSystemParams
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	FCameraComponentParams CameraParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SpringArm")
	FSpringArmLengthParams ArmLengthParams;
	
};