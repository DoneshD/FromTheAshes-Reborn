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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base Params | Tags")
	FGameplayTag UniqueIdentifierTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Params | Tags")
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Params | Animation")
	TObjectPtr<UAnimMontage> MontageToPlay;
};
