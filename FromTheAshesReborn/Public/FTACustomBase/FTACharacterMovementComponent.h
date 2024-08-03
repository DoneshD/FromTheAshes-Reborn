#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "FTACharacterMovementComponent.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UFTACharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()


public:
	UFTACharacterMovementComponent();

	virtual float GetMaxSpeed() const override;

};