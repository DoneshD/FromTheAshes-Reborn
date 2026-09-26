#include "CombatComponents/HeatComponent.h"

UHeatComponent::UHeatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UHeatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UHeatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

