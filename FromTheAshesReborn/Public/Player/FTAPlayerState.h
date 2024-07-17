#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "FTAPlayerState.generated.h"

class UFTAAbilitySystemComponent;
class UFTAAttributeSet;

UCLASS()
class FROMTHEASHESREBORN_API AFTAPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	TObjectPtr<UFTAAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UFTAAttributeSet> AttributeSet;

public:
	AFTAPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UFTAAttributeSet* GetAttributeSet() const;

};
