#pragma once

#include "CoreMinimal.h"
#include "GA_Attack.h"
#include "GA_MeleeAttack.generated.h"

class UMeleeAbilityDataAsset;

USTRUCT(BlueprintType)
struct FMeleeComboType
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UMeleeAbilityDataAsset>> NormalAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UMeleeAbilityDataAsset>> PauseAttacks;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UMeleeAbilityDataAsset>> VariantAttacks;
	
};

UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeAttack : public UGA_Attack
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attacks")
	FMeleeComboType MeleeComboTypes;
};
