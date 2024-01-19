#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType) enum class EShopper : uint8
{

	EState_Nothing		UMETA(DisplayName = "Bagger"),
	EState_Attack		UMETA(DisplayName = "Customer with 25> items in express lane"),
	EState_Dodge		UMETA(DisplayName = "Calling in sick"),
	EState_Execution	UMETA(DisplayName = "Quitting")

};
