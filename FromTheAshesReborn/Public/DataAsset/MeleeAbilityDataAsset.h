﻿#pragma once

#include "CoreMinimal.h"
#include "FTAAbilityDataAsset.h"
#include "CombatComponents/MeleePropertiesComponent.h"
#include "CombatComponents/MeleeWarpingComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack.h"
#include "MeleeAbilityDataAsset.generated.h"

class UGameplayEffect;
class UNiagaraSystem;

UCLASS()
class FROMTHEASHESREBORN_API UMeleeAbilityDataAsset : public UFTAAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Meta Data")
	int RequiredIndex;

	UPROPERTY(EditDefaultsOnly, Category = "Meta Data")
	bool RequiredPause = false;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FMeleeAttackDataStruct AttackData;

	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	TObjectPtr<UHitReactionDataAsset> HitDataAsset;
	
	UPROPERTY(EditDefaultsOnly, Category = "WarpData")
	FMeleeWarpData MeleeWarpData;
};
