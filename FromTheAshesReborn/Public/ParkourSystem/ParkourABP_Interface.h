#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ParkourABP_Interface.generated.h"

struct FGameplayTag;

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UParkourABP_Interface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IParkourABP_Interface
{
	GENERATED_BODY()

public:

	virtual bool SetParkourState(FGameplayTag NewParkourState) = 0;

	virtual bool SetParkourAction(FGameplayTag NewParkourAction) = 0;

	virtual bool SetClimbStyle(FGameplayTag NewClimbStyle) = 0;

	virtual bool SetClimbMovement(FGameplayTag NewDirection) = 0;

	virtual bool SetLeftHandLedgeLocation(FVector LeftHandLedgeLocation) = 0;

	virtual bool SetRightHandLedgeLocation(FVector RightHandLedgeLocation) = 0;

	virtual bool SetLeftFootLocation(FVector LeftFootLocation) = 0;

	virtual bool SetRightFootLocation(FVector RightFootLocation) = 0;

	virtual bool SetLeftHandLedgeRotation(FRotator LeftHandLedgeRotation) = 0;

	virtual bool SetRightHandLedgeRotation(FRotator RightHandLedgeRotation) = 0;

};
