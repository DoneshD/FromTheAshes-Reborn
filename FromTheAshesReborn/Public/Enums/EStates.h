#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType) enum class EStates : uint8
{

	EState_Nothing		UMETA(DisplayName = "Nothing"),
	EState_Attack		UMETA(DisplayName = "Attack"),
	EState_Dash			UMETA(DisplayName = "Dash"),
	EState_Blink		UMETA(DisplayName = "Blink"),
	EState_Execution	UMETA(DisplayName = "Execution")

};
