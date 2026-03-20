#pragma once

#include "CoreMinimal.h"
#include "DataAsset/HitReactionDataAsset.h"
#include "UObject/Object.h"
#include "HitEventObject.generated.h"

class UMoveToLocationDataAsset;
enum class EHitReactionDirection : uint8;
class UGA_Recover;
class UGA_ReceiveHit;

USTRUCT(BlueprintType)
struct FHitInfo
{
	GENERATED_BODY()

	FHitInfo()
		: 
			Instigator(nullptr)
		, HitDirection(ESpatialDirection::Front)
	{}



	UPROPERTY()
	TObjectPtr<AActor> Instigator;

	UPROPERTY()
	ESpatialDirection HitDirection;

	TArray<TSubclassOf<UGA_ReceiveHit>> PossibleFollowupReactions;
	TArray<TSubclassOf<UGA_Recover>> PossibleRecoveryOptions;

	UPROPERTY()
	TObjectPtr<UMoveToLocationDataAsset> MoveToLocationData;
};


UCLASS(Blueprintable, BlueprintType)
class FROMTHEASHESREBORN_API UHitEventObject : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite)
	FHitInfo HitData;
};