#include "Characters/EnemyMelee.h"

AEnemyMelee::AEnemyMelee()
{
	
}

void AEnemyMelee::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyMelee::LightAttack()
{
	this->FindComponentByClass<UAttacksComponent>()->LightMeleeAttack(LightAttackAnim);
}

void AEnemyMelee::NativeGetIdealRange(float& OutAttackRadius, float& OutDefendRadius)
{
    OutAttackRadius = 50.0f;
    OutDefendRadius = 350.0f;
}
