#pragma once

#include "CoreMinimal.h"
#include "AttackAbilityDataAsset.h"
#include "RangedAbilityDataAsset.generated.h"


UCLASS()
class FROMTHEASHESREBORN_API URangedAbilityDataAsset : public UAttackAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Ranged")
	float RangedFloatTest = 0;
};
