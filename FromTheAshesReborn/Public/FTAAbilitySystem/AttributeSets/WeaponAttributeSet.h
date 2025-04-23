#pragma once

#include "CoreMinimal.h"
#include "FTAAttributeSet.h"
#include "WeaponAttributeSet.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UWeaponAttributeSet : public UFTAAttributeSet
{
	GENERATED_BODY()

public:

	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, BaseWeaponDamage);
	
	UWeaponAttributeSet();

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Weapon | Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseWeaponDamage;
	
};
