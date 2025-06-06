#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HitEventObject.generated.h"

USTRUCT(BlueprintType)
struct FHitInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	TObjectPtr<AActor> Instigator;
};

UCLASS()
class FROMTHEASHESREBORN_API UHitEventObject : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite)
	FHitInfo HitData;
};