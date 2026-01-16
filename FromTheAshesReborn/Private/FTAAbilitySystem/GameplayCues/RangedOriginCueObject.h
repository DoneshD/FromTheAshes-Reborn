#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "RangedOriginCueObject.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FRangedOriginCueInfoStruct
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (Categories = "GameplayCue"))
	FGameplayTag OriginCueTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UNiagaraSystem> OriginNiagaraEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TObjectPtr<USoundBase>> OriginSoundsArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinPitch = 0.90f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxPitch = 1.50f;
};

UCLASS()
class FROMTHEASHESREBORN_API URangedOriginCueObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRangedOriginCueInfoStruct RangedOriginCueInfo;
};
