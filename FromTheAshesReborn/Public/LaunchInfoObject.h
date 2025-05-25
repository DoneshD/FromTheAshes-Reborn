#pragma once

#include "CoreMinimal.h"
#include "LaunchInfoObject.generated.h"

USTRUCT(BlueprintType)
struct FLaunchInfo
{
	GENERATED_BODY()

	float VerticalDistance;

	float LaunchDuration;

	float StallDuration;
	
};

UCLASS(BlueprintType)
class FROMTHEASHESREBORN_API ULaunchInfoObject : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLaunchInfo LaunchData;
	
};
