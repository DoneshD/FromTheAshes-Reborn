#include "CameraSystemComponent.h"

#include "HelperFunctionLibraries/ViewportUtilityFunctionLibrary.h"

UCameraSystemComponent::UCameraSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCameraSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	

}

void UCameraSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	

}