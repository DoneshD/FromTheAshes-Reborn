
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType) enum class EDamageType : uint8
{

	EDamageType_None		UMETA(DisplayName = "None"),
	EDamageType_Melee		UMETA(DisplayName = "Melee"),
	EDamageType_Projectile	UMETA(DisplayName = "Projectile"),
	EDamageType_Explosion	UMETA(DisplayName = "Explosion"),
	EDamageType_Environment	UMETA(DisplayName = "Environment")

};