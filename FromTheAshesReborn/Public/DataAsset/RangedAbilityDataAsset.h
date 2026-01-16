#pragma once

#include "CoreMinimal.h"
#include "AttackAbilityDataAsset.h"
#include "RangedAbilityDataAsset.generated.h"

//maybe make this into 4 limb enum
UENUM(BlueprintType)
enum class EHand : uint8
{
	None	UMETA(DisplayName = "None"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
};

UCLASS()
class FROMTHEASHESREBORN_API URangedAbilityDataAsset : public UAttackAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Ranged")
	float RangedFloatTest = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Ranged")
	EHand Hand;

	
};
