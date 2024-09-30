#include "Components/Movement/GroundedDashComponent.h"

UGroundedDashComponent::UGroundedDashComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UGroundedDashComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UGroundedDashComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
