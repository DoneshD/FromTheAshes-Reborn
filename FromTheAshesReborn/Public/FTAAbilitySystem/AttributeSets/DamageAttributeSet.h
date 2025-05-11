#pragma once

#include "CoreMinimal.h"
#include "FTAAttributeSet.h"
#include "DamageAttributeSet.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UDamageAttributeSet : public UFTAAttributeSet
{
	GENERATED_BODY()

public:

	UDamageAttributeSet();
	
	ATTRIBUTE_ACCESSORS(UDamageAttributeSet, BaseDamage);
	
private:
	UPROPERTY(BlueprintReadOnly, Category = "Damage | Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;
};
