#include "CombatComponents/AttackTokenSystemComponent.h"

UAttackTokenSystemComponent::UAttackTokenSystemComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UAttackTokenSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UAttackTokenSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

