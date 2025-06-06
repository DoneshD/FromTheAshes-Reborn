#pragma once
#include "CoreMinimal.h"
#include "HitEventObject.h"
#include "SuspendEventObject.generated.h"

USTRUCT(BlueprintType)
struct FSuspendInfo
{
	GENERATED_BODY()

	float DescentSpeed;
};

UCLASS()
class FROMTHEASHESREBORN_API USuspendEventObject : public UHitEventObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSuspendInfo SuspendData;
};