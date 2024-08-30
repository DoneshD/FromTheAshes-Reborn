#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

//-----------------------------------Ability Input ID-----------------------------------//

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	// 0
	None			UMETA(DisplayName = "None"),
	// 1
	Confirm			UMETA(DisplayName = "Confirm"),
	// 2
	Cancel			UMETA(DisplayName = "Cancel"),
	// 3
	Move			UMETA(DisplayName = "Move"),
	// 4
	Look			UMETA(DisplayName = "Look"),
	// 5
	Jump			UMETA(DisplayName = "Jump"),
	// 6
	Dash			UMETA(DisplayName = "Dash"),
	//7
	LightAttack		UMETA(DisplayName = "LightAttack"),
	//8
	HeavyAttack		UMETA(DisplayName = "HeavyAttack")
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
