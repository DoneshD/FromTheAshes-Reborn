#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType) enum class EAIStates : uint8
{

	EAIStates_Passive		UMETA(DisplayName = "Passive"),
	EAIStates_Attacking		UMETA(DisplayName = "Attacking"),
	EAIStates_Frozen		UMETA(DisplayName = "Frozen"),
	EAIStates_Investigating	UMETA(DisplayName = "Investigating"),
	EAIStates_Dead			UMETA(DisplayName = "Dead")
};
