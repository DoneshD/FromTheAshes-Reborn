#pragma once

#include "CoreMinimal.h"
#include "AttackAbilityDataAsset.h"
#include "CombatComponents/MeleeWarpingComponent.h"
#include "MeleeAbilityDataAsset.generated.h"

class UFTASoundCueObject;
class UGameplayEffect;
class UNiagaraSystem;

UCLASS()
class FROMTHEASHESREBORN_API UMeleeAbilityDataAsset : public UAttackAbilityDataAsset
{
	GENERATED_BODY()

public:

	/*UPROPERTY(EditDefaultsOnly, Category = "Meta Data")
	int RequiredIndex;

	UPROPERTY(EditDefaultsOnly, Category = "Meta Data")
	bool RequiredPause = false;*/

	// UPROPERTY(EditDefaultsOnly, Category = "Attack")
	// FMeleeAttackDataStruct AttackData;
	
	UPROPERTY(EditDefaultsOnly, Category = "WarpData")
	FMeleeWarpData MeleeWarpData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slash")
	TSubclassOf<UFTASoundCueObject> SlashSoundCue;

};
