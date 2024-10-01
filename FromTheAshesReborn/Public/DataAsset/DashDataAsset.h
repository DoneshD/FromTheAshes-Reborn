#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DashDataAsset.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UDashDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DashIndentiferTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditDefaultsOnly)
	int RequiredIndex;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> MontageToPlay;
};
