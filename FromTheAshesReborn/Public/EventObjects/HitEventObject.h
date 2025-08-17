#pragma once

#include "CoreMinimal.h"
#include "DataAsset/HitReactionDataAsset.h"
#include "UObject/Object.h"
#include "HitEventObject.generated.h"

enum class EHitReactionDirection : uint8;

USTRUCT(BlueprintType)
struct FHitInfo
{
	GENERATED_BODY()

	FHitInfo()
		: Location(FVector::ZeroVector)
		, Instigator(nullptr)
		, HitDirection(ESpatialDirection::Front)
	{}

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	TObjectPtr<AActor> Instigator;

	UPROPERTY()
	ESpatialDirection HitDirection;
};


UCLASS()
class FROMTHEASHESREBORN_API UHitEventObject : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite)
	FHitInfo HitData;
};