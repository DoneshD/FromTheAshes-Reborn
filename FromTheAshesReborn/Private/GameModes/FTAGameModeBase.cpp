#include "GameModes/FTAGameModeBase.h"

#include "Components/AudioComponent.h"

AFTAGameModeBase::AFTAGameModeBase()
{
	
}

void AFTAGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentEncounter = 0;
	
}

void AFTAGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
