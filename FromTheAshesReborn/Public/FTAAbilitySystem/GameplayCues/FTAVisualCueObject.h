#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "FTAVisualCueObject.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FVisualCueStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FVector Location;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FRotator Rotation;
	
};

UCLASS(BlueprintType, Blueprintable)
class FROMTHEASHESREBORN_API UFTAVisualCueObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (Categories = "GameplayCue"))
	FGameplayTag VisualCueTag;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<FVisualCueStruct> VisualCueArray;
};
