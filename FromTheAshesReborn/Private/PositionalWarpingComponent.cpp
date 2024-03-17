#include "PositionalWarpingComponent.h"

UPositionalWarpingComponent::UPositionalWarpingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPositionalWarpingComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UPositionalWarpingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

