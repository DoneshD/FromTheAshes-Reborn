#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayCues/FTACueObject.h"
#include "OverlayCueObject.generated.h"

class UMaterialInterface;

USTRUCT(BlueprintType)
struct FOverlayStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> MaterialInterface;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AlphaSpeed = 3.0f;
	
};


UCLASS()
class FROMTHEASHESREBORN_API UOverlayCueObject : public UFTACueObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FOverlayStruct OverlayStruct;
	
};
