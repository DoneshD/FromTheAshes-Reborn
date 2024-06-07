#include "Characters/EnemyBase.h"
#include "AI/Controllers/AIControllerEnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DamageSystem/DamageSystem.h"
#include "Components/ArrowComponent.h"
#include "CombatComponents/AttackTokenSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"
#include "GameModes/FromTheAshesRebornGameMode.h"
#include "Components/CapsuleComponent.h"
#include "Interfaces/EventManagerInterface.h"
#include "EventManagers/CombatManager.h"
#include "MotionWarpingComponent.h"
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

	IEventManagerInterface* EventManagerInterface = Cast<IEventManagerInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	EventManagerInterface->PublishEnemySpawned();

	EventManagerInterface->HandleAttackRequest(this);

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

float AEnemyBase::SetMovementSpeed(EMovementSpeed SpeedState)
{
	return 0.0f;
}

void AEnemyBase::GetIdealRange(float& OutAttackRadius, float& OutDefendRadius, float& OutOrbitRadius)
{
	OutOrbitRadius = 1500.0f;
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

void AEnemyBase::FinishLightMeleeAttack()
{
	OnAttackEnd.Execute();
}

void AEnemyBase::HandleDeath()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AICEnemyBase->GetBrainComponent()->StopLogic(TEXT(""));
	AICEnemyBase->SetStateAsDead();

	PlayAnimMontage(DeathMontage);

	IEventManagerInterface* EventManagerInterface = Cast<IEventManagerInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	EventManagerInterface->PublishEnemyDeath();

	EventManagerInterface->HandleDeath(this);
}

void AEnemyBase::HandleHitReaction(FDamageInfo DamageInfo)
{
	IEventManagerInterface* EventManagerInterface = Cast<IEventManagerInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	EventManagerInterface->HandleAttackerSwapRequest(this);

	if (AICEnemyBase->GetCurrentState() != EAIStates::EAIStates_Frozen)
	{
		AICEnemyBase->PriorState = GetCurrentState();
	}

	AICEnemyBase->SetStateAsFrozen();

	DamageSystemComponent->HandleHitReaction(DamageInfo);
}

void AEnemyBase::SetStateAsAttacking(TObjectPtr<AActor> AttackTarget)
{
	AICEnemyBase->SetStateAsAttacking(AttackTarget, true);
}

void AEnemyBase::SetStateAsOrbiting(TObjectPtr<AActor> AttackTarget)
{
	AICEnemyBase->SetStateAsOrbiting(AttackTarget);
}

EAIStates AEnemyBase::GetCurrentState()
{
	return AICEnemyBase->GetCurrentState();
}


void AEnemyBase::Retreat()
{
	AICEnemyBase->SetStateAsPassive();
}

bool AEnemyBase::AttackStart(AActor* AttackTarget, int TokensNeeded)
{
	
	return AttackTokenSystemComponent->AttackStart(AttackTarget, TokensNeeded);
}

void AEnemyBase::AttackEnd(AActor* AttackTarget)
{
	AttackTokenSystemComponent->AttackEnd(AttackTarget);
}

bool AEnemyBase::ReserveAttackToken(int Amount)
{
	return AttackTokenSystemComponent->ReserveAttackToken(Amount);
}

void AEnemyBase::ReturnAttackToken(int Amount)
{
	AttackTokenSystemComponent->ReturnAttackToken(Amount);
}

void AEnemyBase::StoreAttackTokens(AActor* AttackTarget, int Amount)
{
	AttackTokenSystemComponent->StoreAttackTokens(AttackTarget, Amount);
}


void AEnemyBase::FunctionToExecuteOnAnimationBlendOut(UAnimMontage* animMontage, bool bInterrupted)
{
	if (bInterrupted)
	{
		OnAttackEnd.Execute();
	}
	else 
	{
		OnAttackEnd.Execute();

	}
}

void AEnemyBase::FunctionToExecuteOnAnimationEnd(UAnimMontage* animMontage, bool bInterrupted)
{

}

void AEnemyBase::PlayAnAnimationMontage(UAnimMontage* montageToPlay)
{
	if (montageToPlay)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(montageToPlay);

		FOnMontageEnded BlendOutDelegate;
		BlendOutDelegate.BindUObject(this, &AEnemyBase::FunctionToExecuteOnAnimationBlendOut);
		GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendOutDelegate, montageToPlay);

		FOnMontageEnded CompleteDelegate;
		CompleteDelegate.BindUObject(this, &AEnemyBase::FunctionToExecuteOnAnimationEnd);
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(CompleteDelegate, montageToPlay);
	}
}

