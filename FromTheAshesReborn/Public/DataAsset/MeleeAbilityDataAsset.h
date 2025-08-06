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

	UPROPERTY(EditDefaultsOnly)
	int RequiredIndex;

	UPROPERTY(EditDefaultsOnly)
	bool RequiredPause = false;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> HitEffect;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag HitReactionTag;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> HitVFxImpact;

	UPROPERTY(EditDefaultsOnly)
	float TraceSize;

	UPROPERTY(EditDefaultsOnly)
	FMeleeMeleeWarpData MeleeWarpData;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> SlashFX;
};
