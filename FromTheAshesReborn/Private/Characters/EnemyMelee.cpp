#include "Characters/EnemyMelee.h"

AEnemyMelee::AEnemyMelee()
{
	
}

void AEnemyMelee::BeginPlay()
{
	Super::BeginPlay();
	
}


void AEnemyMelee::NativeGetIdealRange(float& OutAttackRadius, float& OutDefendRadius)
{
	OutAttackRadius = 50.0f;
	OutDefendRadius = 350.0f;
}

void AEnemyMelee::LightAttack(AActor* AttackTarget)
{
	IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(AttackTarget);

	if (DamagableInterface)
	{
		if (DamagableInterface->ReserveAttackToken(1))
		{
			this->FindComponentByClass<UAttacksComponent>()->LightMeleeAttack(LightAttackAnim);
		}
		else
		{
			this->FindComponentByClass<UAttacksComponent>()->OnAttackEnd.Execute();
		}
	}
}