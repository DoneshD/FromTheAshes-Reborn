#pragma once

#include "CoreMinimal.h"
#include "FTAAbilityDataAsset.h"
#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack.h"
#include "MeleeAbilityDataAsset.generated.h"

class UGameplayEffect;
class UNiagaraSystem;

UCLASS()
class FROMTHEASHESREBORN_API UMeleeAbilityDataAsset : public UFTAAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Required")
	int RequiredIndex;

	UPROPERTY(EditDefaultsOnly, Category = "Required")
	bool RequiredPause = false;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> HitGameplayEffect;

	UPROPERTY(EditDefaultsOnly,  Category = "Gameplay Effect")
	FGameplayTag HitReactionTag;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> HitEffectImpact;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> SlashEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Range")
	float TraceSize;
	
	UPROPERTY(EditDefaultsOnly, Category = "WarpData")
	FMeleeWarpData MeleeWarpData;
	
};
