#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FTAAbilityDataObject.generated.h"

// USTRUCT(BlueprintType)
// struct FHitInfo
// {
// 	GENERATED_BODY()
// 	
// 	UPROPERTY()
// 	FVector Location;
//
// 	
// };

USTRUCT(BlueprintType)
struct FAbilityDataStruct
{
	GENERATED_BODY()

public:

	
	
};

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityRuntimeDataSetSignature, FAbilityDataStruct, InAbilityData);

UCLASS(Blueprintable, BlueprintType)
class FROMTHEASHESREBORN_API UFTAAbilityDataObject : public UObject
{
	GENERATED_BODY()

public:

	// UPROPERTY(BlueprintAssignable, Category="Melee")
	// // FOnAbilityRuntimeDataSetSignature OnAbilityRuntimeDataSet;
	
	
};