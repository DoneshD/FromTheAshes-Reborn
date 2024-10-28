#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ParkourStatsInterface.generated.h"

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UParkourStatsInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IParkourStatsInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	virtual bool SetParkourState(FText ParkourState) = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool SetParkourAction(FText ParkourAction) = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool SetClimbStyle(FText ClimbStyle) = 0;

};
