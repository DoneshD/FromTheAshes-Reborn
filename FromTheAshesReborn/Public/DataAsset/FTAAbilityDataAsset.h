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

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base Params | Unique Tag")
	FGameplayTag UniqueIdentifierTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Params | Animation")
	TObjectPtr<UAnimMontage> MontageToPlay;

	UPROPERTY(EditDefaultsOnly, Category = "Base Params | Combo Requirements")
	int RequiredIndex;

	UPROPERTY(EditDefaultsOnly, Category = "Base Params | Combo Requirements")
	bool CheckForRequirementTags = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Params | Combo Requirements")
	FGameplayTagContainer RequiredComboTags;
	
	UPROPERTY(EditDefaultsOnly, Category = "Base Params | Combo Requirements")
	bool RequiredPause = false;
	
	// UPROPERTY(EditDefaultsOnly, Category = "WarpData")
	// FMeleeWarpData MeleeWarpData;
};
