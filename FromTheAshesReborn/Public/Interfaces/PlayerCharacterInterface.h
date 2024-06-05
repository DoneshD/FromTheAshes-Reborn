#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerCharacterInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IPlayerCharacterInterface
{
	GENERATED_BODY()

public:

	virtual bool GetHasMovementInput() = 0;
};
