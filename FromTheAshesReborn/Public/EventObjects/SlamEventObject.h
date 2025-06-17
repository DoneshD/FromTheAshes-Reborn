#pragma once

#include "CoreMinimal.h"
#include "HitEventObject.h"
#include "SlamEventObject.generated.h"


USTRUCT(BlueprintType)
struct FSlamInfo
{
	GENERATED_BODY()
	
};

UCLASS()
class FROMTHEASHESREBORN_API USlamEventObject : public UHitEventObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlamInfo SlamData;
	
};
