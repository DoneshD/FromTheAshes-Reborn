#include "FTAAbilitySystem/FTAAbilitySystemComponent.h"


UFTAAbilitySystemComponent::UFTAAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);

}


void UFTAAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UFTAAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

