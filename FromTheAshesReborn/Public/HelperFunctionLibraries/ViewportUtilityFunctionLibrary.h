#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Player/PlayerCharacter.h"
#include "ViewportUtilityFunctionLibrary.generated.h"


UCLASS()
class FROMTHEASHESREBORN_API UViewportUtilityFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector2D FindCenterOfViewPort(UWorld* World, float SpringArmXOffset = 40.0f);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsInViewport(UWorld* World, const AActor* ActorToCheck, const APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool PlayerSideRelativeToActorOnScreen(UWorld* World, const AActor* ActorToCheck, const APlayerCharacter* PlayerCharacter, const APlayerController* PlayerController);
};
