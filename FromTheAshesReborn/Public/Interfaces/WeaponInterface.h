#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

class AFTAWeapon;

UINTERFACE()
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IWeaponInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual AFTAWeapon* GetLightWeapon() = 0;

	UFUNCTION()
	virtual AFTAWeapon* GetHeavyWeapon() = 0;
};
