// Copyright Epic Games, Inc. All Rights Reserved.

#include "FromTheAshesRebornGameMode.h"
#include "FromTheAshesRebornCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFromTheAshesRebornGameMode::AFromTheAshesRebornGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
