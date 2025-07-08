#include "HelperFunctionLibraries/ViewportUtilityFunctionLibrary.h"

#include "Player/PlayerCharacter.h"

FVector2D UViewportUtilityFunctionLibrary::FindCenterOfViewPort(const float SpringArmXOffset)
{
	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

	UE_LOG(LogTemp, Error, TEXT("UViewportUtilityFunctionLibrary: SpringArmXOffset %f"), SpringArmXOffset);
	
	const FVector2D CenterViewportSize = FVector2D((ViewportSize.X * 0.5f) - SpringArmXOffset, ViewportSize.Y * 0.5f);
	
	return CenterViewportSize;
}

bool UViewportUtilityFunctionLibrary::IsInViewport(const AActor* ActorToCheck, const APlayerController* PlayerController)
{
	// if (!IsValid(PlayerController))
	// {
	// 	return false;
	// }

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("UViewportUtilityFunctionLibrary::IsInViewport - PlayerController is NULL"));
		return false;
	}

	FVector2D ScreenLocation;
	PlayerController->ProjectWorldLocationToScreen(ActorToCheck->GetActorLocation(), ScreenLocation);

	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

	return ScreenLocation.X > 0 && ScreenLocation.Y > 0 && ScreenLocation.X < ViewportSize.X && ScreenLocation.Y < ViewportSize.Y;
}

bool UViewportUtilityFunctionLibrary::PlayerSideRelativeToActorOnScreen(UWorld* World, const AActor* ActorToCheck, const APlayerCharacter* PlayerCharacter, const APlayerController* PlayerController)
{
	if (!IsValid(ActorToCheck))
	{
		UE_LOG(LogTemp, Error, TEXT("UViewportUtilityFunctionLibrary::IsInViewport - ActorToCheck is NULL or invalid."));
		return false;
	}

	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("UViewportUtilityFunctionLibrary::IsInViewport - PlayerCharacter is NULL or invalid."));
		return false;
	}

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("UViewportUtilityFunctionLibrary::IsInViewport - PlayerController is NULL or invalid."));
		return false;
	}
	
	FVector2D ViewportSize;
	World->GetGameViewport()->GetViewportSize(ViewportSize);

	FVector2D PlayerScreenLocation;
	PlayerController->ProjectWorldLocationToScreen(PlayerCharacter->GetActorLocation(), PlayerScreenLocation);

	FVector2D TargetScreenLocation;
	PlayerController->ProjectWorldLocationToScreen(ActorToCheck->GetActorLocation(), TargetScreenLocation);

	const float PlayerX = PlayerScreenLocation.X;
	const float TargetX = TargetScreenLocation.X;

	return PlayerX < TargetX;

}
