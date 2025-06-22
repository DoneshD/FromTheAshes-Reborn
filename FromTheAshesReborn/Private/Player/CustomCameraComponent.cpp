#include "Player/CustomCameraComponent.h"

UCustomCameraComponent::UCustomCameraComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

}

void UCustomCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCustomCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

