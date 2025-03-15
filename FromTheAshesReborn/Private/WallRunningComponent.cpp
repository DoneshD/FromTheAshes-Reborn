#include "WallRunningComponent.h"

UWallRunningComponent::UWallRunningComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UWallRunningComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UWallRunningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UWallRunningComponent::CheckForWall()
{
	return true;
}

bool UWallRunningComponent::CheckWallRunning()
{
	return false;
}

void UWallRunningComponent::WallRunningMovement()
{
	
}

