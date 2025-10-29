#include "GameModes/FTAGameModeBase.h"

#include "Components/AudioComponent.h"

AFTAGameModeBase::AFTAGameModeBase()
{
	CombatMusic = CreateDefaultSubobject<UAudioComponent>(TEXT("CombatMusic"));
	CombatMusic->SetupAttachment(RootComponent);
}

void AFTAGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if(PlayMusic)
	{
		if(CombatMusic && !CombatMusic->IsPlaying())
		{
			CombatMusic->Play();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Combat Music is null or already playing"));
		}
	}
	else
	{
		CombatMusic->Stop();
	}
	
}
