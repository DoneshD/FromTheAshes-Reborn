#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "MeleeAttackDataAsset.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UMeleeAttackDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AttackIndentiferTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditDefaultsOnly)
	int RequiredIndex;

	UPROPERTY(EditDefaultsOnly)
	bool RequiredPause = false;

	UPROPERTY(EditDefaultsOnly)
	bool IsComboFinisher = false;

	UPROPERTY(EditDefaultsOnly)
	bool ShouldEmptyComboContainer;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> MontageToPlay;

	
};
