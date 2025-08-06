#pragma once

#include "CoreMinimal.h"
#include "EnumHelpers.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FTAAbilityDataAsset.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UFTAAbilityDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Direction")
	ESpatialDirection Direction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability Asset Tag")
	FGameplayTag UniqueIdentifierTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Asset Tag")
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UAnimMontage> MontageToPlay;
	
};
