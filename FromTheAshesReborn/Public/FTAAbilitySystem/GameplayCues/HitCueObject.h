#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "HitCueObject.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FHitCueInfoStruct
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (Categories = "GameplayCue"))
	FGameplayTag HitCueTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UNiagaraSystem> HitNiagaraEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UNiagaraSystem> BloodTrailNiagaraEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TObjectPtr<USoundBase>> HitSoundsArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinPitch = 0.90f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxPitch = 1.50f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float HitStopDuration = 0.05f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float HitStopDilation = 0.50f;
};


UCLASS(BlueprintType, Blueprintable)
class FROMTHEASHESREBORN_API UHitCueObject : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FHitCueInfoStruct HitCueInfo;
	
};
