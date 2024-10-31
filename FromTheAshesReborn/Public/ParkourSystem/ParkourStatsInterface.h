#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ParkourStatsInterface.generated.h"

UINTERFACE()
class UParkourStatsInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IParkourStatsInterface
{
	GENERATED_BODY()

public:

	virtual bool SetParkourState(FText ParkourState) = 0;

	virtual bool SetParkourAction(FText ParkourAction) = 0;

	virtual bool SetClimbStyle(FText ClimbStyle) = 0;

};
