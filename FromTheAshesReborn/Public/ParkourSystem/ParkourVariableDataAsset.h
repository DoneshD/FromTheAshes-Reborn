
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ParkourVariableDataAsset.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UParkourVariableDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag VaultDepthTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag VaultHeightTag;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> ParkourMontage;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InParkourStateTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag OutParkourStateTag;

	UPROPERTY(EditDefaultsOnly)
	float Warp1XOffset;

	UPROPERTY(EditDefaultsOnly)
	float Warp1ZOffset;
	
	UPROPERTY(EditDefaultsOnly)
	float Warp2XOffset;
	
	UPROPERTY(EditDefaultsOnly)
	float Warp2ZOffset;

	UPROPERTY(EditDefaultsOnly)
	float Warp3XOffset;

	UPROPERTY(EditDefaultsOnly)
	float Warp3ZOffset;

	UPROPERTY(EditDefaultsOnly)
	float MontageStartPosition;

	UPROPERTY(EditDefaultsOnly)
	float FallingMontageStartPosition;
};
