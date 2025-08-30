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

FHitReactionStruct UHitReactionComponent::GetAssociatedHitReaction(FGameplayTag Index)
{
	for (FHitReactionStruct HitReaction : HitReactions)
	{
		if(HitReaction.HitTag == Index)
		{
			return HitReaction;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("HitReaction does not exist"));
	return FHitReactionStruct();
}

