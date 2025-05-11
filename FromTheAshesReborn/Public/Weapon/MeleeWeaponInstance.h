#pragma once

#include "CoreMinimal.h"
#include "WeaponInstance.h"
#include "MeleeWeaponInstance.generated.h"


UCLASS()
class FROMTHEASHESREBORN_API UMeleeWeaponInstance : public UWeaponInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Config")
	float MaxDamage = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Config")
	FRuntimeFloatCurve DistanceDamageFalloff;
	
};
