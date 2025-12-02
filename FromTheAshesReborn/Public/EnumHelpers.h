#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ESpatialDirection : uint8 {
	Front		UMETA(DisplayName = "Front"),
	Back		UMETA(DisplayName = "Back"),
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right"),
	Up			UMETA(DisplayName = "Up"),
	Down		UMETA(DisplayName = "Down"),
	Any			UMETA(DisplayName = "Any"),
	Special			UMETA(DisplayName = "Special"),
	TempBlocker			UMETA(DisplayName = "TempBlocker"),
	Knockback			UMETA(DisplayName = "Knockback"),
	None		UMETA(DisplayName = "None")
};

