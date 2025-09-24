#pragma once

#include "CoreMinimal.h"
#include "FTAAbilityDataAsset.h"
#include "HitReactionDataAsset.generated.h"

class UGameplayEffect;


UCLASS()
class FROMTHEASHESREBORN_API UHitReactionDataAsset : public UFTAAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Hit", meta = (Categories="Character"))
	FGameplayTag Orientation;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	ESpatialDirection Direction;
	
};
