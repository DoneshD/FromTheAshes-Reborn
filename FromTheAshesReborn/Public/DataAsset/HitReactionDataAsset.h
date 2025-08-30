#pragma once

#include "CoreMinimal.h"
#include "FTAAbilityDataAsset.h"
#include "CombatComponents/HitReactionComponent.h"
#include "HitReactionDataAsset.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UHitReactionDataAsset : public UFTAAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (Categories = "HitTag"))
	ESpatialDirection HitReactionDirection;

	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	FHitReactionDataStruct HitReactionData;
	
};
