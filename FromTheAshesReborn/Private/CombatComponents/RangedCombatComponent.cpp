#include "CombatComponents/RangedCombatComponent.h"

URangedCombatComponent::URangedCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void URangedCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void URangedCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}