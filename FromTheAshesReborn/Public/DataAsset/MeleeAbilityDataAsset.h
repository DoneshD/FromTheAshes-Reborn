#pragma once

#include "CoreMinimal.h"
#include "AttackAbilityDataAsset.h"
#include "MeleeAbilityDataAsset.generated.h"

class UFTASoundCueObject;
class UGameplayEffect;
class UNiagaraSystem;

UCLASS()
class FROMTHEASHESREBORN_API UMeleeAbilityDataAsset : public UAttackAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee | Trail")
	TArray<TSubclassOf<UWeaponCueObject>> TrailVisualCueClassArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee | Trail")
	TArray<TSubclassOf<UFTASoundCueObject>> TrailSoundCueClassArray;
};
