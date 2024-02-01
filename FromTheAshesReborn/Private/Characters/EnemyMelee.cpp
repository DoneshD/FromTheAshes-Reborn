#include "Characters/EnemyMelee.h"
#include "GameFramework/CharacterMovementComponent.h"


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

float AEnemyMelee::NativeSetMovementSpeed(EMovementSpeed SpeedState)
{
	UE_LOG(LogTemp, Warning, TEXT("AEnemyMelee::NativeSetMovementSpeed"));

	switch (SpeedState)
	{
	case EMovementSpeed::EMovementSpeed_Idle:
		return GetCharacterMovement()->MaxWalkSpeed = 0.0f;

	case EMovementSpeed::EMovementSpeed_Walking:
		return GetCharacterMovement()->MaxWalkSpeed = 100.0f;

	case EMovementSpeed::EMovementSpeed_Jogging:
		return GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	case EMovementSpeed::EMovementSpeed_Sprinting:
		return GetCharacterMovement()->MaxWalkSpeed = 500.0f;

	default:
		return 0.0f;
		break;
	}
}

void AEnemyMelee::LightAttack(AActor* AttackTarget)
{
	IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(AttackTarget);
	UE_LOG(LogTemp, Warning, TEXT("AEnemyMelee::LightAttack"));

	if (DamagableInterface)
	{
		if (DamagableInterface->ReserveAttackToken(1))
		{
			UE_LOG(LogTemp, Warning, TEXT("DamagableInterface->ReserveAttackToken(1) is true"));

			this->FindComponentByClass<UAttacksComponent>()->LightMeleeAttack(LightAttackAnim);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DamagableInterface->ReserveAttackToken(1) is false"));

			this->FindComponentByClass<UAttacksComponent>()->OnAttackEnd.Execute();
		}
	}
}