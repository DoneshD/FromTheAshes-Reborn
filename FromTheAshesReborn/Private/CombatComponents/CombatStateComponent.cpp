#include "CombatComponents/CombatStateComponent.h"

UCombatStateComponent::UCombatStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCombatStateComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	

}