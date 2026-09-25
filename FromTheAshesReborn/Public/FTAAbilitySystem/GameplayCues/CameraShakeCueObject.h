#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayCues/FTACueObject.h"
#include "CameraShakeCueObject.generated.h"

USTRUCT(BlueprintType)
struct FCameraShakeStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> CameraShakeBaseClass;
	
};

UCLASS()
class FROMTHEASHESREBORN_API UCameraShakeCueObject : public UFTACueObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FCameraShakeStruct CameraShakeStruct;
	
};
