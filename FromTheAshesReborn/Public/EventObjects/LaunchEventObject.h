#pragma once

#include "CoreMinimal.h"
#include "HitEventObject.h"
#include "UObject/Object.h"
#include "LaunchEventObject.generated.h"

USTRUCT(BlueprintType)
struct FLaunchInfo
{
	GENERATED_BODY()

public:
	mutable float VerticalDistance;
	mutable float LaunchDuration;
	mutable float StallDuration;
	mutable float Offset;
	
};

UCLASS()
class FROMTHEASHESREBORN_API ULaunchEventObject : public UHitEventObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLaunchInfo LaunchData;
	
};
