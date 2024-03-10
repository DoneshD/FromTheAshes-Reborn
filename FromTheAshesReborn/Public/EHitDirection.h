#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType) enum class EHitDirection : uint8
{
	EHitDirection_None		UMETA(DisplayName = "None"),
	EHitDirection_Left	UMETA(DisplayName = "Left"),
	EHitDirection_Right		UMETA(DisplayName = "Right"),
	EHitDirection_Front	UMETA(DisplayName = "Front"),
	EHitDirection_Back	UMETA(DisplayName = "Back"),
	EHitDirection_FrontLeft	UMETA(DisplayName = "FrontLeft"),
	EHitDirection_FrontRight	UMETA(DisplayName = "FrontRight"),
	EHitDirection_BackLeft	UMETA(DisplayName = "BackLeft"),
	EHitDirection_BackRight	UMETA(DisplayName = "BackRight")

};
