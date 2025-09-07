#pragma once

#include "CoreMinimal.h"
#include "FTAAbilityDataAsset.h"
#include "HitReactionDataAsset.generated.h"

class UGameplayEffect;

// USTRUCT(BlueprintType)
// struct FHitReactionDataStruct
// {
// 	GENERATED_BODY()
// 	
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
// 	TSubclassOf<UGameplayEffect> HitEffect;
//
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
// 	FGameplayTag HitTag;
//
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
// 	FGameplayTag CharacterOrientationTag;
// 	
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
// 	FGameplayTagContainer CharacterStateTags;
// 	
// 	FHitReactionDataStruct()
// 	:
// 	HitEffect(nullptr),
// 	HitTag(FGameplayTag::EmptyTag),
// 	CharacterOrientationTag(FGameplayTag::EmptyTag),
// 	CharacterStateTags(FGameplayTag::EmptyTag)
// 	{}
// 	
// };

UCLASS()
class FROMTHEASHESREBORN_API UHitReactionDataAsset : public UFTAAbilityDataAsset
{
	GENERATED_BODY()

public:

	// UPROPERTY(EditDefaultsOnly, Category = "Hit")
	// FHitReactionDataStruct HitReactionData;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	ESpatialDirection HitReactionDirection;
	
};
