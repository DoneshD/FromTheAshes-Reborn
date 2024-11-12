#pragma once

#include "CoreMinimal.h"
#include "FTAWeaponInstance.h"
#include "FTAMeleeWeaponInstance.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UFTAMeleeWeaponInstance : public UFTAWeaponInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Config")
	int32 DamagePerSwing = 1;
};
