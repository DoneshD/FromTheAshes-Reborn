#pragma once

#include "CoreMinimal.h"
#include "CameraSystemParams.generated.h"


USTRUCT(BlueprintType)
struct FCameraSystemParams
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool ShouldAdjustArmLength = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool ShouldOverrideArmLength = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool ShouldResetOffset = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DeltaArmLength = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DeltaArmLengthInterpSpeed = 5.0f;
	
};