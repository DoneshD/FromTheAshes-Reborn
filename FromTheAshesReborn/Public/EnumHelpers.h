#pragma once
#include "CoreMinimal.h"

class UNiagaraSystem;

UENUM(BlueprintType)
enum class ESpatialDirection : uint8 {
	Front		UMETA(DisplayName = "Front"),
	Back		UMETA(DisplayName = "Back"),
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right"),
	Up			UMETA(DisplayName = "Up"),
	Down		UMETA(DisplayName = "Down"),
	None		UMETA(DisplayName = "None")
};