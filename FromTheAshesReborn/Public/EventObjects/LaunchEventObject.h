#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LaunchEventObject.generated.h"

USTRUCT(BlueprintType)
struct FLaunchInfo
{
	GENERATED_BODY()

	float VerticalDistance;

	float LaunchDuration;

	float StallDuration;
	
};

UCLASS()
class FROMTHEASHESREBORN_API ULaunchEventObject : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLaunchInfo LaunchData;
	
};
