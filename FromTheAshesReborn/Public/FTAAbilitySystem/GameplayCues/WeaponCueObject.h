#pragma once

#include "CoreMinimal.h"
#include "FTAVisualCueObject.h"
#include "WeaponCueObject.generated.h"

class AWeaponActorBase;

UCLASS()
class FROMTHEASHESREBORN_API UWeaponCueObject : public UFTAVisualCueObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<AWeaponActorBase> WeaponActorClass;
};
