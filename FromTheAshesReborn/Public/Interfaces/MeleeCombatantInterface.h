#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "MeleeCombatantInterface.generated.h"

// struct FGameplayTagContainer;

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

	virtual void RegisterGameplayTagEvent(FGameplayTag ComboWindowTag, FTimerHandle& FComboWindowTimer) = 0;

	virtual void RemoveGameplayTagEvent(FGameplayTag ComboWindowTag) = 0;

	
	
};
