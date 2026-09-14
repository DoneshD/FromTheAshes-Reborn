#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayCues/FTACueObject.h"
#include "DecalCueObject.generated.h"

USTRUCT(BlueprintType)
struct FDecalStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UMaterialInterface* DecalMaterial;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FVector Size;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float Duration = 5.0f;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FVector Location;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FVector LocationOffset;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FRotator Rotation;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FRotator RotationOffset;
	
};

UCLASS()
class FROMTHEASHESREBORN_API UDecalCueObject : public UFTACueObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<FDecalStruct> DecalCueArray;
	
};
