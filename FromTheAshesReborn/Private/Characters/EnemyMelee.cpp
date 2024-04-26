#include "Characters/EnemyMelee.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CombatComponents/MeleeAttackLogicComponent.h"
#include "Weapons/MeleeWeapon.h"

AEnemyMelee::AEnemyMelee()
{
	MeleeAttackLogicComponent = CreateDefaultSubobject<UMeleeAttackLogicComponent>(TEXT("MeleeAttackLogicComponent"));
	this->AddOwnedComponent(MeleeAttackLogicComponent);
}

void AEnemyMelee::BeginPlay()
{
	Super::BeginPlay();

	if (MeleeWeapon_L = GetWorld()->SpawnActor<AMeleeWeapon>(MeleeWeaponClass, GetMesh()->GetSocketTransform(TEXT("hand_l_enemy_weapon_socket"))))
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
		MeleeWeapon_L->AttachToComponent(GetMesh(), AttachmentRules, TEXT("hand_l_enemy_weapon_socket"));
	}

	if (MeleeWeapon_R = GetWorld()->SpawnActor<AMeleeWeapon>(MeleeWeaponClass, GetMesh()->GetSocketTransform(TEXT("hand_r_enemy_weapon_socket"))))
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
		MeleeWeapon_R->AttachToComponent(GetMesh(), AttachmentRules, TEXT("hand_r_enemy_weapon_socket"));
	}
	
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
	PlayAnimMontage(LightAttackAnim);
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
	MeleeAttackLogicComponent->EmptyHitActorsArray();
}

void AEnemyMelee::StartMeleeAttackCollisions()
{
	MeleeAttackLogicComponent->StartMeleeAttackCollisions();
}

void AEnemyMelee::EndMeleeAttackCollisions()
{
	MeleeAttackLogicComponent->EndMeleeAttackCollisions();
}

bool AEnemyMelee::MeleeWeaponSphereTrace(FVector StartLocation, FVector EndLocation, TArray<FHitResult>& Hits)
{
	return MeleeAttackLogicComponent->MeleeWeaponSphereTrace(StartLocation, EndLocation, Hits);
}

void AEnemyMelee::MeleeTraceCollisions()
{
	MeleeAttackLogicComponent->MeleeTraceCollisions();
}

void AEnemyMelee::MeleeAttackWarpToTarget()
{
	MeleeAttackLogicComponent->MeleeAttackWarpToTarget(MeleeAttackLogicComponent->MeleeAttackRange, false);
}

void AEnemyMelee::ResetMeleeAttackWarpToTarget()
{
	MeleeAttackLogicComponent->ResetMeleeAttackWarpToTarget();
}

