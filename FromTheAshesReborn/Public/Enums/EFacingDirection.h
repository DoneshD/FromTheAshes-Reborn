#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType) enum class EFacingDirection : uint8
{
	EFacingDirection_None		UMETA(DisplayName = "None"),
	EFacingDirection_Left		UMETA(DisplayName = "Left"),
	EFacingDirection_Right		UMETA(DisplayName = "Right"),
	EFacingDirection_Front		UMETA(DisplayName = "Front"),
	EFacingDirection_Back		UMETA(DisplayName = "Back"),
	EFacingDirection_FrontLeft	UMETA(DisplayName = "FrontLeft"),
	EFacingDirection_FrontRight	UMETA(DisplayName = "FrontRight"),
	EFacingDirection_BackLeft	UMETA(DisplayName = "BackLeft"),
	EFacingDirection_BackRight	UMETA(DisplayName = "BackRight")
};
