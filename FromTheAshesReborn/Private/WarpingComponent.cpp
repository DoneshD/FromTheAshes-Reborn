#include "WarpingComponent.h"

UWarpingComponent::UWarpingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UWarpingComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UWarpingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

