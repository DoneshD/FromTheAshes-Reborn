#pragma once

#include "CoreMinimal.h"
#include "FTAAbilityDataAsset.h"
#include "DashAbilityDataAsset.generated.h"


enum class ELockOnInputOrientationDirection : uint8;

UCLASS()
class FROMTHEASHESREBORN_API UDashAbilityDataAsset : public UFTAAbilityDataAsset
{
	GENERATED_BODY()

public:
	//TODO: Change later
	UPROPERTY(EditDefaultsOnly)
	ELockOnInputOrientationDirection DashDirection;
};
