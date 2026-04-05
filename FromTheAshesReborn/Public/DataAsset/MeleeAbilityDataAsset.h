#pragma once

#include "CoreMinimal.h"
#include "AttackAbilityDataAsset.h"
#include "MeleeAbilityDataAsset.generated.h"

class UFTASoundCueObject;
class UGameplayEffect;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class ELimb : uint8
{
	LeftArm,
	RightArm,
	LeftLeg,
	RightLeg
};

UENUM(BlueprintType)
enum class EMeleeType : uint8
{
	Weapon,
	Limb
};

USTRUCT(BlueprintType)
struct FMeleeSource
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMeleeType MeleeType = EMeleeType::Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MeleeType == EMeleeType::Limb", EditConditionHides))
	ELimb Limb;
};

UCLASS()
class FROMTHEASHESREBORN_API UMeleeAbilityDataAsset : public UAttackAbilityDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee")
	FMeleeSource MeleeSource;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee")
	TArray<TSubclassOf<UWeaponCueObject>> TrailVisualCueClassArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee")
	TArray<TSubclassOf<UFTASoundCueObject>> TrailSoundCueClassArray;

};
