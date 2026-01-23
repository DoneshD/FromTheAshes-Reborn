#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "FTASoundCueObject.generated.h"

USTRUCT(BlueprintType)
struct FSoundCueStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USoundBase> SoundsArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float VolumeMultiplier = 1.0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinPitch = 1.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxPitch = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StartTime = 0.0;
	
};

UCLASS(BlueprintType, Blueprintable)
class FROMTHEASHESREBORN_API UFTASoundCueObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (Categories = "GameplayCue"))
	FGameplayTag SoundCueTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FSoundCueStruct> SoundCueArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DefaultVolumeMultiplier = 1.0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DefaultMinPitch = 1.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DefaultMaxPitch = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DefaultStartTime = 0.0;
};
