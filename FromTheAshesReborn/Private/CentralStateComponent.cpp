#include "CentralStateComponent.h"

UCentralStateComponent::UCentralStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCentralStateComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCentralStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

