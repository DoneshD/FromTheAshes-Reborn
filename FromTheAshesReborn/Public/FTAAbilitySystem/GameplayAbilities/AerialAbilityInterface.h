#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AerialAbilityInterface.generated.h"

UINTERFACE()
class UAerialAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IAerialAbilityInterface
{
	GENERATED_BODY()

	
public:

	virtual bool IsCharacterAirborne() const = 0;
};
