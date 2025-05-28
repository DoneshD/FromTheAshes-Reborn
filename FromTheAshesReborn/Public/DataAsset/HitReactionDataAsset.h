#pragma once

#include "CoreMinimal.h"
#include "FTAAbilityDataAsset.h"
#include "HitReactionDataAsset.generated.h"

UENUM(BlueprintType)
enum class EHitReactionDirection : uint8
{
	Front		UMETA(DisplayName = "Front"),
	Back		UMETA(DisplayName = "Back"),
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right"),
	Up			UMETA(DisplayName = "Up"),
	Down		UMETA(DisplayName = "Down")
};
UCLASS()
class FROMTHEASHESREBORN_API UHitReactionDataAsset : public UFTAAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (Categories = "HitTag"))
	EHitReactionDirection HitReactionDirection;
	
};
