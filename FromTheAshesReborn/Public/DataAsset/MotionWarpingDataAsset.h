#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MotionWarpingDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FWarpData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WarpTarget")
	FName WarpTargetName = NAME_None;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WarpTarget")
	TObjectPtr<AActor> WarpTargetActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TraceData")
	float StartTraceLocationOffset = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TraceData")
	float TraceDistance = 800.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TraceData")
	float TraceRadius = 600.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Location")
	FVector WarpTargetLocation = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Location")
	float WarpTargetLocationOffset = 0.0f;
};


UCLASS()
class FROMTHEASHESREBORN_API UMotionWarpingDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TraceData")
	FWarpData WarpData;
	
};
