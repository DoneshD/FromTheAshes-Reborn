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
	HeavyAttack		UMETA(DisplayName = "HeavyAttack"),
	//9
	LockOn			UMETA(DisplayName = "LockOn"),
	//10
	Vault			UMETA(DisplayName = "Vault")
};


UENUM(BlueprintType)
enum class EAllowedInputs : uint8
{
	// 0
	None			UMETA(DisplayName = "None"),
	
	Jump			UMETA(DisplayName = "Jump"),
	// 1
	Dash			UMETA(DisplayName = "Dash"),
	//2
	LightAttack		UMETA(DisplayName = "LightAttack"),
	//3
	HeavyAttack		UMETA(DisplayName = "HeavyAttack")
};
