#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AerialAbilityModifier.generated.h"

class UAerialAbilityDataAsset;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class FROMTHEASHESREBORN_API UAerialAbilityModifier : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAerialAbilityDataAsset> AerialData;

	bool CanActivate(TObjectPtr<ACharacter> Character) const;

	bool IsCharacterAirborne(TObjectPtr<ACharacter> Character) const;
};
