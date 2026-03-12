#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AerialAbilityModifier.generated.h"

class UAerialAbilityDataAsset;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class FROMTHEASHESREBORN_API UAerialAbilityModifier : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)

	TObjectPtr<UAerialAbilityDataAsset> AerialData;

	bool CanActivate() const;
};
