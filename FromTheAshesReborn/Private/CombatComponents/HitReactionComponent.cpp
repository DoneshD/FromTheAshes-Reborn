#include "CombatComponents/HitReactionComponent.h"

UHitReactionComponent::UHitReactionComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

}


void UHitReactionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UHitReactionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

FHitReactionDataStruct UHitReactionComponent::GetAssociatedHitReaction(FGameplayTag Index)
{
	for (FHitReactionDataStruct HitReaction : HitReactions)
	{
		if(HitReaction.HitTag == Index)
		{
			return HitReaction;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("HitReaction does not exist"));
	return FHitReactionDataStruct();
}

