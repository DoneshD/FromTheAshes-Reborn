#pragma once

#include "CoreMinimal.h"
#include "FTAGameplayAbility.h"
#include "DataAsset/MeleeAttackDataAsset.h"
#include "GA_MeleeAttack.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeAttack : public UFTAGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UMeleeAttackDataAsset>> LightAttacks;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UMeleeAttackDataAsset>> HeavyAttacks;
};
