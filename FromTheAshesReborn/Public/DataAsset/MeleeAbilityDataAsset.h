#pragma once

#include "CoreMinimal.h"
#include "FTAAbilityDataAsset.h"
#include "MeleeAbilityDataAsset.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UMeleeAbilityDataAsset : public UFTAAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	int RequiredIndex;

	UPROPERTY(EditDefaultsOnly)
	bool RequiredPause = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (Categories = "HitTag"))
	FGameplayTag HitReaction;
};
