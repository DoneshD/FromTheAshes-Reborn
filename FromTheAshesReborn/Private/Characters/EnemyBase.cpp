#include "Characters/EnemyBase.h"
#include "AI/Controllers/AIControllerEnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DamageSystem/DamageSystem.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameModes/FTAGameStateBase.h"

#include "CombatManager.h"
#include "AIController.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
}
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	AController* EnemyController = this->GetController();
	if (EnemyController)
	{
		AAIController* AIEnemyController = Cast<AAIController>(EnemyController);
		if (AIEnemyController)
		{
			AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(AIEnemyController);
			if (AIControllerEnemyBase)
			{

				AICEnemyBase = AIControllerEnemyBase;
			}
		}
	}

	CombatManager = Cast<ACombatManager>(UGameplayStatics::GetActorOfClass(GetWorld(), CombatManagerClass));
	CombatManager->HandleAttackRequest(this);

	FTAGameStateBase = Cast<AFTAGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));

	DamageSystemComponent->OnDeathResponse.BindUObject(this, &AEnemyBase::HandleDeath);
	DamageSystemComponent->OnDamageResponse.AddUObject(this, &AEnemyBase::HandleHitReaction);

}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemyBase::GetCurrentHealth()
{
	return DamageSystemComponent->CurrentHealth;
}

float AEnemyBase::GetMaxHealth()
{
	return DamageSystemComponent->MaxHealth;
}

bool AEnemyBase::IsDead()
{
	return DamageSystemComponent->IsDead;
}

float AEnemyBase::Heal(float HealAmount)
{
	return DamageSystemComponent->Heal(HealAmount);
}

bool AEnemyBase::TakeDamage(FDamageInfo DamageInfo)
{
	return DamageSystemComponent->TakeDamage(DamageInfo);
}

bool AEnemyBase::ReserveAttackToken(int Amount)
{
	return DamageSystemComponent->ReserveAttackTokens(Amount);
}

void AEnemyBase::ReturnAttackToken(int Amount)
{
	DamageSystemComponent->ReturnAttackTokens(Amount);
}

float AEnemyBase::SetMovementSpeed(EMovementSpeed SpeedState)
{
	return 0.0f;
}

void AEnemyBase::GetIdealRange(float& OutAttackRadius, float& OutDefendRadius)
{
	
}

void AEnemyBase::JumpToDestination(FVector Destination)
{
	FVector LaunchVelocity;
	FVector EndDestination = Destination + FVector(0.0f, 0.0f, 250.0f);
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), LaunchVelocity, GetActorLocation(), Destination);
	LaunchCharacter(LaunchVelocity, true, true);
}

void AEnemyBase::LightAttack()
{

}

bool AEnemyBase::AttackStart(AActor* AttackTarget, int TokensNeeded)
{
	IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(AttackTarget);
	if (DamagableInterface)
	{
		if (DamagableInterface->ReserveAttackToken(TokensNeeded))
		{
			StoreAttackTokens(AttackTarget, TokensNeeded);
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

void AEnemyBase::AttackEnd(AActor* AttackTarget)
{
	IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(AttackTarget);
	if (DamagableInterface)
	{
		DamagableInterface->ReturnAttackToken(TokensUsedInCurrentAttack);
		StoreAttackTokens(AttackTarget, -1 * TokensUsedInCurrentAttack);
	}
}

void AEnemyBase::FinishLightMeleeAttack()
{
	OnAttackEnd.Execute();
}

void AEnemyBase::HandleDeath()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AICEnemyBase->GetBrainComponent()->StopLogic(TEXT(""));
	PlayAnimMontage(DeathMontage);
	AICEnemyBase->SetStateAsDead();
	CombatManager->HandleDeath(this);
}

void AEnemyBase::HandleHitReaction(FDamageInfo DamageInfo)
{
	GetCharacterMovement()->StopMovementImmediately();
	AICEnemyBase->SetStateAsFrozen();

	UAnimMontage* HitReactionMontage = nullptr;

	switch (DamageInfo.FacingDirection)
	{
	case EFacingDirection::EFacingDirection_Left:
		HitReactionMontage = RightHitReaction;
		break;

	case EFacingDirection::EFacingDirection_Right:
		HitReactionMontage = LeftHitReaction;
		break;

	case EFacingDirection::EFacingDirection_Front:
		HitReactionMontage = FrontHitReaction;
		break;

	case EFacingDirection::EFacingDirection_Back:
		HitReactionMontage = BackHitReaction;
		break;

	case EFacingDirection::EFacingDirection_FrontLeft:
		HitReactionMontage = FrontHitReaction;
		break;

	case EFacingDirection::EFacingDirection_FrontRight:
		HitReactionMontage = FrontHitReaction;
		break;

	case EFacingDirection::EFacingDirection_BackLeft:
		HitReactionMontage = BackHitReaction;
		break;

	case EFacingDirection::EFacingDirection_BackRight:
		HitReactionMontage = BackHitReaction;
		break;

	default:
		break;
	}

	if (HitReactionMontage)
	{
		PlayAnimMontage(HitReactionMontage);
	}
}

void AEnemyBase::StoreAttackTokens(AActor* AttackTarget, int Amount)
{
	if (ReservedAttackTokensMap.Find(AttackTarget))
	{
		Amount += ReservedAttackTokensMap[AttackTarget];

	}

	ReservedAttackTokensMap.Add(AttackTarget, Amount);
}

void AEnemyBase::Attack(TObjectPtr<AActor> AttackTarget)
{
	AICEnemyBase->SetStateAsAttacking(AttackTarget, true);
}

void AEnemyBase::Wait(TObjectPtr<AActor> AttackTarget)
{
	AICEnemyBase->SetStateAsWaiting(AttackTarget);
}

void AEnemyBase::Retreat()
{
	AICEnemyBase->SetStateAsPassive();
}
