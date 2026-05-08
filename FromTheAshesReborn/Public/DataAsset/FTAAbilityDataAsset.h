#pragma once

#include "CoreMinimal.h"
#include "EnumHelpers.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FTAAbilityDataAsset.generated.h"

class UCameraParamsDataAsset;
class AWeaponActorBase;

UCLASS()
class FROMTHEASHESREBORN_API UFTAAbilityDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Base Params | Unique Tag")
	FGameplayTag UniqueIdentifierTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Params | Weapon")
	TSubclassOf<AWeaponActorBase> WeaponActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Params | Camera")
	TObjectPtr<UCameraParamsDataAsset> CameraParamsDataAsset;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Params | Animation")
	TObjectPtr<UAnimMontage> MontageToPlay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Params | Animation")
	bool EndAbilityOnCompleted = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Params | Animation")
	bool IncrementComboIndexOnCompleted = false;

	UPROPERTY(EditDefaultsOnly, Category = "Base Params | Combo Requirements")
	int RequiredIndex;

	UPROPERTY(EditDefaultsOnly, Category = "Base Params | Combo Requirements")
	bool CheckForRequirementTags = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Params | Combo Requirements")
	FGameplayTagContainer RequiredComboTags;
	
	UPROPERTY(EditDefaultsOnly, Category = "Base Params | Combo Requirements")
	bool RequiredPause = false;
	
};
