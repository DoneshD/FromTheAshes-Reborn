#include "Characters/EnemyMelee.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CombatComponents/MeleeAttackLogicComponent.h"
#include "MeleeWeapon.h"

AEnemyMelee::AEnemyMelee()
{
	MeleeAttackLogicComponent = CreateDefaultSubobject<UMeleeAttackLogicComponent>(TEXT("MeleeAttackLogicComponent"));
	this->AddOwnedComponent(MeleeAttackLogicComponent);
}

void AEnemyMelee::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyMelee::GetIdealRange(float& OutAttackRadius, float& OutDefendRadius)
{
	OutAttackRadius = 50.0f;
	OutDefendRadius = 600.0f;
}

float AEnemyMelee::SetMovementSpeed(EMovementSpeed SpeedState)
{
	switch (SpeedState)
	{
	case EMovementSpeed::EMovementSpeed_Idle:
		return GetCharacterMovement()->MaxWalkSpeed = 0.0f;

	case EMovementSpeed::EMovementSpeed_Walking:
		return GetCharacterMovement()->MaxWalkSpeed = 200.0f;

	case EMovementSpeed::EMovementSpeed_Jogging:
		return GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	case EMovementSpeed::EMovementSpeed_Sprinting:
		return GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	default:
		return 0.0f;
		break;
	}
}

void AEnemyMelee::LightAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Should not be here"));
	//this->FindComponentByClass<UAttacksComponent>()->LightMeleeAttack(LightAttackAnim);
}

AMeleeWeapon* AEnemyMelee::GetLeftWeapon()
{
	return MeleeWeapon_L;
}

AMeleeWeapon* AEnemyMelee::GetRightWeapon()
{
	return MeleeWeapon_R;
}
void AEnemyMelee::EmptyHitActorsArray()
{

}

void AEnemyMelee::StartMeleeAttackCollisions()
{

}

void AEnemyMelee::EndMeleeAttackCollisions()
{

}

bool AEnemyMelee::MeleeWeaponSphereTrace(FVector StartLocation, FVector EndLocation, TArray<FHitResult>& Hits)
{
	return false;
}

void AEnemyMelee::MeleeTraceCollisions()
{
	return;
}

void AEnemyMelee::MeleeAttackWarpToTarget()
{
	return;
}

void AEnemyMelee::ResetMeleeAttackWarpToTarget()
{
	return;
}

