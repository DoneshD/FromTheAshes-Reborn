#include "CombatComponents/ComboSystemComponent.h"
#include "Enums/EStates.h"
#include "Animation/AnimInstance.h"

UComboSystemComponent::UComboSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UComboSystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UComboSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

