#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

//-----------------------------------Ability Input ID-----------------------------------//

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None			UMETA(DisplayName = "None"),
	Confirm			UMETA(DisplayName = "Confirm"),
	Cancel			UMETA(DisplayName = "Cancel"),
	Move			UMETA(DisplayName = "Move"),
	Look			UMETA(DisplayName = "Look"),
	Jump			UMETA(DisplayName = "Jump"),
	Dash			UMETA(DisplayName = "Dash"),
	LightAttack		UMETA(DisplayName = "LightAttack")
};

//-----------------------------------Hit Reaction Direction-----------------------------------//

UENUM(BlueprintType)
enum class EHitReactDirection : uint8
{
	// 0
	None			UMETA(DisplayName = "None"),
	// 1
	Left 			UMETA(DisplayName = "Left"),
	// 2
	Front 			UMETA(DisplayName = "Front"),
	// 3
	Right			UMETA(DisplayName = "Right"),
	// 4
	Back			UMETA(DisplayName = "Back")
};
