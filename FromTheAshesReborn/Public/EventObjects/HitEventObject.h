#pragma once

#include "CoreMinimal.h"
#include "DataAsset/HitReactionDataAsset.h"
#include "UObject/Object.h"
#include "HitEventObject.generated.h"

class UMoveToLocationDataAsset;
enum class EHitReactionDirection : uint8;
class UGA_Recover;
class UGA_ReceiveHit;


UCLASS(Blueprintable, BlueprintType)
class FROMTHEASHESREBORN_API UHitEventObject : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite)
	FHitDataInfo HitData;
};