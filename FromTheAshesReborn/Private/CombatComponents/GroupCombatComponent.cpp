#include "CombatComponents/GroupCombatComponent.h"

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

bool UGroupCombatComponent::ReserveAttackToken(int32 Amount)
{
	if(AttackTokensCount >= Amount)
	{
		AttackTokensCount -= Amount;
		return true;
	}
	return false;
}

void UGroupCombatComponent::ReturnAttackToken(int32 Amount)
{
	AttackTokensCount += Amount;
}
