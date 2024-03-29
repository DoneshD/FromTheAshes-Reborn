#include "DamageSystem/DamageSystem.h"

UDamageSystem::UDamageSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageSystem::BeginPlay()
{
	Super::BeginPlay();

}

void UDamageSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

ECanBeDamaged UDamageSystem::CanBeDamaged(bool ShouldDamageInvincible, bool CanBeBlocked)
{
	if (!IsDead && ShouldDamageInvincible || !IsInvincible)
	{
		if (IsBlocking && CanBeBlocked)
		{
			return ECanBeDamaged::ECanBeDamaged_BlockedDamage;
		}
		else
		{
			return ECanBeDamaged::ECanBeDamaged_DoDamage;
		}
	}
	else
	{
		return ECanBeDamaged::ECanBeDamaged_NoDamage;
	}
}

float UDamageSystem::Heal(float HealAmount)
{
	if (!IsDead)
	{
		CurrentHealth += HealAmount;
		if (CurrentHealth > MaxHealth)
		{
			CurrentHealth = MaxHealth;
		}
		return CurrentHealth;
	}
	return 0;
}

bool UDamageSystem::TakeDamage(FDamageInfo DamageInfo)
{
	
	DamageOutput = CanBeDamaged(DamageInfo.ShouldDamageInvincible, DamageInfo.CanBeBlocked);
	if (DamageOutput == ECanBeDamaged::ECanBeDamaged_BlockedDamage)
	{
		//CallOnBlocked(DamageInfo.CanBeParried)
		return true;
	}
	else if (DamageOutput == ECanBeDamaged::ECanBeDamaged_DoDamage)
	{
		CurrentHealth -= DamageInfo.DamageAmount;
		if (CurrentHealth <= 0)
		{
			IsDead = true;
			//CallOnDeath
			return true;
		}
		else
		{
			if (DamageInfo.ShouldForceInterrupt || IsInterruptible)
			{
				OnDamageResponse.Broadcast(DamageInfo);
				return true;
			}
			return true;
		}
	}
	else if (DamageOutput == ECanBeDamaged::ECanBeDamaged_NoDamage)
	{
		return false;
	}
	else
	{
		return false;
	}
}

bool UDamageSystem::ReserveAttackTokens(int Amount)
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

void UDamageSystem::ReturnAttackTokens(int Amount)
{
	AttackTokensCount += Amount;
}
