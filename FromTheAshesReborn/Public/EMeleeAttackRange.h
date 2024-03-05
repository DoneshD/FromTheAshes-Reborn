#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType) enum class EMeleeAttackRange : uint8
{
	EMeleeAttackRange_Close		UMETA(DisplayName = "Close"),
	EMeleeAttackRange_Mid		UMETA(DisplayName = "Mid"),
	EMeleeAttackRange_Far		UMETA(DisplayName = "Far"),
};

