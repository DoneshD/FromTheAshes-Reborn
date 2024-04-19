#include "GroupCombatComponent.h"

UGroupCombatComponent::UGroupCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UGroupCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UGroupCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

