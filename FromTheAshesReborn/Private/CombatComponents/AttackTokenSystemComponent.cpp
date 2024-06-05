#include "CombatComponents/AttackTokenSystemComponent.h"
#include "Interfaces/AttackTokenSystemInterface.h"

UAttackTokenSystemComponent::UAttackTokenSystemComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}


void UAttackTokenSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UAttackTokenSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UAttackTokenSystemComponent::AttackStart(AActor* AttackTarget, int TokensNeeded)
{
	IAttackTokenSystemInterface* AttackTokenSystemInterface = Cast<IAttackTokenSystemInterface>(AttackTarget);
	if (AttackTokenSystemInterface)
	{
		if (AttackTokenSystemInterface->ReserveAttackToken(TokensNeeded))
		{
			AttackTokenSystemInterface->StoreAttackTokens(AttackTarget, TokensNeeded);
			TokensUsedInCurrentAttack = TokensNeeded;
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
	
}

void UAttackTokenSystemComponent::AttackEnd(AActor* AttackTarget)
{
	IAttackTokenSystemInterface* AttackTokenSystemInterface = Cast<IAttackTokenSystemInterface>(AttackTarget);

	if (AttackTokenSystemInterface)
	{
		AttackTokenSystemInterface->ReturnAttackToken(TokensUsedInCurrentAttack);
		StoreAttackTokens(AttackTarget, -1 * TokensUsedInCurrentAttack);
	}
}

bool UAttackTokenSystemComponent::ReserveAttackToken(int Amount)
{
	if (AttackTokensCount >= Amount)
	{
		AttackTokensCount -= Amount;
		return true;
	}
	else
	{
		return false;
	}
}

void UAttackTokenSystemComponent::ReturnAttackToken(int Amount)
{
	AttackTokensCount += Amount;
}

void UAttackTokenSystemComponent::StoreAttackTokens(AActor* AttackTarget, int Amount)
{
	if (ReservedAttackTokensMap.Find(AttackTarget))
	{
		Amount += ReservedAttackTokensMap[AttackTarget];

	}

	ReservedAttackTokensMap.Add(AttackTarget, Amount);
}

