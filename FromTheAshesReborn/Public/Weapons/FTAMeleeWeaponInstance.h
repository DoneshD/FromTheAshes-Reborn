#pragma once

#include "CoreMinimal.h"
#include "FTAWeaponInstance.h"
#include "FTAMeleeWeaponInstance.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UFTAMeleeWeaponInstance : public UFTAWeaponInstance
{
	GENERATED_BODY()

public:
	
	UFTAMeleeWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Config")
	float MaxDamage = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Config")
	FRuntimeFloatCurve DistanceDamageFalloff;

	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;
};
