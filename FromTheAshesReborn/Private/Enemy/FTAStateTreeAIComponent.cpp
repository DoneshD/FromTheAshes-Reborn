#include "Enemy/FTAStateTreeAIComponent.h"



UFTAStateTreeAIComponent::UFTAStateTreeAIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UFTAStateTreeAIComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UFTAStateTreeAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UFTAStateTreeAIComponent::SetStateTree(UStateTree* InStateTree)
{
	if(IsRunning())
	{
		Cleanup();
	}
	StateTreeRef.SetStateTree(InStateTree);
}

