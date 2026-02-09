#pragma once

#include "CoreMinimal.h"
#include "FTAAbilityDataAsset.h"
#include "HitReactionDataAsset.generated.h"

class UGA_ReceiveHit;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FHitDataInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UGA_ReceiveHit> HitAbilityClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ESpatialDirection Direction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Distance;
	
};

UCLASS()
class FROMTHEASHESREBORN_API UHitReactionDataAsset : public UFTAAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FHitDataInfo HitData;
	
};
