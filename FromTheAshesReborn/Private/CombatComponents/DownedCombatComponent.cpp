#include "CombatComponents/DownedCombatComponent.h"

UDownedCombatComponent::UDownedCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UDownedCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UDownedCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}