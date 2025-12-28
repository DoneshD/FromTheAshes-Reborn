#pragma once

#include "CoreMinimal.h"
#include "MoviePlayer.h"
#include "Engine/GameInstance.h"
#include "FTAGameInstance.generated.h"



UCLASS()
class FROMTHEASHESREBORN_API UFTAGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	virtual void Init() override;

	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& MapName);
	
	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);
	
};