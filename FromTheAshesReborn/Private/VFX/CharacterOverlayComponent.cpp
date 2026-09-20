#include "VFX/CharacterOverlayComponent.h"

UCharacterOverlayComponent::UCharacterOverlayComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCharacterOverlayComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCharacterOverlayComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
