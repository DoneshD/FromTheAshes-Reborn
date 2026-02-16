#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CameraParamsDataAsset.generated.h"

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

	UPROPERTY(EditAnywhere, Category = "Catch up")
	float CatchupInterpSpeed = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance Based Offset")
	float DistanceBasedMaxPitchOffset = -20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance Based Offset")
	float DistanceBasedMaxYawOffset = -35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Rotation")
	bool ShouldUpdateControllerRotation = false;
};
