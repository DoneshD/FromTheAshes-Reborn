#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "FTAVisualCueObject.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FNiagaraCueStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FVector Location;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FVector LocationOffset;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FRotator Rotation;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FRotator RotationOffset;
	
};

USTRUCT(BlueprintType)
struct FDecalCueStruct
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

UCLASS(BlueprintType, Blueprintable)
class FROMTHEASHESREBORN_API UFTAVisualCueObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (Categories = "GameplayCue"))
	FGameplayTag VisualCueTag;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<FNiagaraCueStruct> NiagaraCueArray;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<FDecalCueStruct> DecalCueArray;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool ActivateAllCues = false;
};
