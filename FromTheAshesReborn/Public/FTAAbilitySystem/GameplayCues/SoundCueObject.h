#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayCues/FTACueObject.h"
#include "SoundCueObject.generated.h"

USTRUCT(BlueprintType)
struct FSoundCueStruc
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USoundBase> Sound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float VolumeMultiplier = 1.0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinPitch = 1.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxPitch = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StartTime = 0.0;
	
};

UCLASS()
class FROMTHEASHESREBORN_API USoundCueObject : public UFTACueObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FSoundCueStruc> SoundCueArray;
	
};
