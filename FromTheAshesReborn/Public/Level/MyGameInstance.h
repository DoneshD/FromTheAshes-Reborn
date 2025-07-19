#pragma once

#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& MapName);
	
	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);
};
