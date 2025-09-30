#include "Enemy/AIPropertiesComponent.h"

UAIPropertiesComponent::UAIPropertiesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UAIPropertiesComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAIPropertiesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

