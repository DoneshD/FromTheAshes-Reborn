#include "HelperFunctionLibraries/ViewportUtilityFunctionLibrary.h"

#include "Player/PlayerCharacter.h"

FVector2D UViewportUtilityFunctionLibrary::FindCenterOfViewPort(UWorld* World, const float SpringArmXOffset)
{
	FVector2D ViewportSize;
	World->GetGameViewport()->GetViewportSize(ViewportSize);
	
	const FVector2D CenterViewportSize = FVector2D((ViewportSize.X * 0.5f) - SpringArmXOffset, ViewportSize.Y * 0.5f);
	return CenterViewportSize;
}

bool UViewportUtilityFunctionLibrary::IsInViewport(UWorld* World, FVector Location, const APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("UViewportUtilityFunctionLibrary::IsInViewport - PlayerController is NULL"));
		return false;
	}

	FVector2D ScreenLocation;
	
	if (!PlayerController->ProjectWorldLocationToScreen(Location, ScreenLocation))
	{
		return false;
	}

	FVector2D ViewportSize;
	World->GetGameViewport()->GetViewportSize(ViewportSize);

	const bool bIsOnScreen = 
		ScreenLocation.X > 0 && ScreenLocation.Y > 0 &&
		ScreenLocation.X < ViewportSize.X && 
		ScreenLocation.Y < ViewportSize.Y;

	return bIsOnScreen;
}


bool UViewportUtilityFunctionLibrary::PlayerSideRelativeToLocationOnScreen(UWorld* World, FVector Location, const APlayerCharacter* PlayerCharacter, const APlayerController* PlayerController)
{
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("UViewportUtilityFunctionLibrary::PlayerSideRelativeToActorOnScreen - PlayerCharacter is NULL or invalid."));
		return false;
	}

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("UViewportUtilityFunctionLibrary::PlayerSideRelativeToActorOnScreen - PlayerController is NULL or invalid."));
		return false;
	}
	
	FVector2D ViewportSize;
	World->GetGameViewport()->GetViewportSize(ViewportSize);

	FVector2D PlayerScreenLocation;
	PlayerController->ProjectWorldLocationToScreen(PlayerCharacter->GetActorLocation(), PlayerScreenLocation);

	FVector2D TargetScreenLocation;
	PlayerController->ProjectWorldLocationToScreen(Location, TargetScreenLocation);

	const float PlayerX = PlayerScreenLocation.X;
	const float TargetX = TargetScreenLocation.X;

	return PlayerX < TargetX;

}
