#include "Level/FTAGameInstance.h"

#include "MoviePlayer.h"
#include "Level/BaseLoadingScreen.h"

void UFTAGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UFTAGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UFTAGameInstance::EndLoadingScreen);
	
	
}

void UFTAGameInstance::BeginLoadingScreen(const FString& MapName)
{
	if (!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.MinimumLoadingScreenDisplayTime = 3.0f;

		LoadingScreen.WidgetLoadingScreen = SNew(BaseLoadingScreen);

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
		GetMoviePlayer()->PlayMovie();
	}
}

void UFTAGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
	UE_LOG(LogTemp, Warning, TEXT("EndLoadingScreen"))
}
