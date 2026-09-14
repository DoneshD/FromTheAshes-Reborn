#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayCues/FTACueObject.h"
#include "MaterialCueObject.generated.h"

USTRUCT(BlueprintType)
struct FMaterialStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> MaterialInterface;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 ElementIndex;
	
};


UCLASS()
class FROMTHEASHESREBORN_API UMaterialCueObject : public UFTACueObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FMaterialStruct> MaterialStructArray;
	
	
};
