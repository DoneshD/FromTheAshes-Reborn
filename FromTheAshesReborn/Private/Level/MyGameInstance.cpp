#include "Level/MyGameInstance.h"
#include "MoviePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "Level/SMyLoadingScreen.h"

void UMyGameInstance::Init()
{
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UMyGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UMyGameInstance::EndLoadingScreen);
	
}

void UMyGameInstance::BeginLoadingScreen(const FString& MapName)
{
	if (!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.MinimumLoadingScreenDisplayTime = 3.0f;

		LoadingScreen.WidgetLoadingScreen = SNew(SMyLoadingScreen);

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
		GetMoviePlayer()->PlayMovie();
	}
}

void UMyGameInstance::EndLoadingScreen(UWorld* LoadedWorld)
{
	UE_LOG(LogTemp, Warning, TEXT("EndLoadingScreen"));
}
