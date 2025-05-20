#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FTAAbilityDataAsset.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UFTAAbilityDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability Asset Tag", Meta = (Categories = "AbilityAssetTag"))
	FGameplayTag UniqueIdentifierTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Asset Tag", Meta = (Categories = "AbilityAssetTag"))
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Asset Tag")
	TObjectPtr<UAnimMontage> MontageToPlay;
	
};
