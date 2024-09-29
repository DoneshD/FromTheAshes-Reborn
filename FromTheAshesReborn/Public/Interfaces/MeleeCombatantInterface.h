#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MeleeCombatantInterface.generated.h"

struct FGameplayTagContainer;

UINTERFACE()
class UMeleeCombatantInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IMeleeCombatantInterface
{
	GENERATED_BODY()

public:

	virtual FGameplayTagContainer& GetCurrentComboContainer() = 0;

	virtual int GetCurrentComboIndex() = 0;

	virtual void SetCurrentComboIndex(int ComboIndex) = 0;
};
