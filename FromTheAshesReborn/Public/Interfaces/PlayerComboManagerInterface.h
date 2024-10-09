#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "PlayerComboManagerInterface.generated.h"

UINTERFACE()
class UPlayerComboManagerInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IPlayerComboManagerInterface
{
	GENERATED_BODY()

public:
	virtual FGameplayTagContainer& GetCurrentComboContainer() = 0;

	virtual int GetCurrentComboIndex() = 0;

	virtual void SetCurrentComboIndex(int ComboIndex) = 0;

	virtual void RegisterWindowGameplayTagEvent(FGameplayTag ComboWindowTag, FTimerHandle& FComboWindowTimer) = 0;

	virtual void RemoveWindowGameplayTagEvent(FGameplayTag ComboWindowTag, FTimerHandle& ComboWindowTimer) = 0;

};
