// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemyBase.h"
#include "AI/Controllers/AIControllerEnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DamageSystem/DamageSystem.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	DamageSystemComponent->OnDeathResponse.BindUObject(this, &AEnemyBase::HandleDeathReaction);
	DamageSystemComponent->OnDamageResponse.AddUObject(this, &AEnemyBase::HandleHitReaction);

}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//not good!!!!!!!
	if (AICEnemyBase->AttackTarget)
	{
		//SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AICEnemyBase->AttackTarget->GetActorLocation()));
	}

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
		UE_LOG(LogTemp, Warning, TEXT(" AEnemyBase::AttackEnd"));

		DamagableInterface->ReturnAttackToken(TokensUsedInCurrentAttack);
		UE_LOG(LogTemp, Warning, TEXT(" AEnemyBase::AttackEnd %d"), TokensUsedInCurrentAttack);
		StoreAttackTokens(AttackTarget, -1 * TokensUsedInCurrentAttack);
	}
}

void AEnemyBase::FinishLightMeleeAttack()
{
	OnAttackEnd.Execute();
}

void AEnemyBase::HandleDeathReaction()
{
	PlayAnimMontage(DeathReaction);
}

void AEnemyBase::HandleHitReaction(FDamageInfo DamageInfo)
{
	GetCharacterMovement()->StopMovementImmediately();
	AICEnemyBase->SetStateAsFrozen();

	UAnimMontage* HitReactionMontage = nullptr;

	if (DamageSystemComponent->CurrentHealth < 0)
	{

	}

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

void AEnemyBase::OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	AICEnemyBase->SetStateAsAttacking(AICEnemyBase->AttackTarget, true);
}

void AEnemyBase::OnMontageInterrupted(UAnimMontage* Montage, bool bInterrupted)
{

}

void AEnemyBase::StoreAttackTokens(AActor* AttackTarget, int Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("AEnemyBase--StoreAttackTokens OURSIDE::Amount: %d"), Amount);

	if (ReservedAttackTokensMap.Find(AttackTarget))
	{
		Amount += ReservedAttackTokensMap[AttackTarget];
		UE_LOG(LogTemp, Warning, TEXT("AEnemyBase--StoreAttackTokens INSIDE::Amount: %d"), Amount);

	}

	ReservedAttackTokensMap.Add(AttackTarget, Amount);
}

EHitDirection AEnemyBase::GetHitEnemyDirection(FVector HitLocation)
{
	TArray<float> DistanceArray;
	float ClosestArrowDistance = 1000.0f;
	int ClosestArrowIndex = 0;

	DistanceArray.Add(FVector::Dist(HitLocation, LeftArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(HitLocation, RightArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(HitLocation, FrontArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(HitLocation, BackArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(HitLocation, FrontLeftArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(HitLocation, FrontRightArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(HitLocation, BackLeftArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(HitLocation, BackRightArrow->GetComponentLocation()));

	for (const float& EachArrowDistance : DistanceArray)
	{
		if (EachArrowDistance < ClosestArrowDistance)
		{
			ClosestArrowDistance = EachArrowDistance;
			ClosestArrowIndex = DistanceArray.Find(EachArrowDistance);
		}
	}
	switch (ClosestArrowIndex)
	{
	case 0:
		return EHitDirection::EHitDirection_Left;

	case 1:
		return EHitDirection::EHitDirection_Right;

	case 2:
		return EHitDirection::EHitDirection_Front;

	case 3:
		return EHitDirection::EHitDirection_Back;

	case 4:
		return EHitDirection::EHitDirection_FrontLeft;

	case 5:
		return EHitDirection::EHitDirection_FrontRight;

	case 6:
		return EHitDirection::EHitDirection_BackLeft;

	case 7:
		return EHitDirection::EHitDirection_BackRight;

	default:
		break;
	}
	return EHitDirection::EHitDirection_None;
}

void AEnemyBase::Attack(TObjectPtr<AActor> AttackTarget)
{
	UE_LOG(LogTemp, Warning, TEXT("Attack"));
	//AICEnemyBase->SetStateAsAttacking(AttackTarget, true);
}

void AEnemyBase::Wait(TObjectPtr<AActor> AttackTarget)
{
	UE_LOG(LogTemp, Warning, TEXT("Wait"));
	//AICEnemyBase->SetStateAsPassive();
}

void AEnemyBase::Retreat()
{
	UE_LOG(LogTemp, Warning, TEXT("Retreat"));
	//AICEnemyBase->SetStateAsPassive();
}

void AEnemyBase::UpdateWarpTarget(FMotionWarpingTarget& MotionWarpingTargetParams)
{

}

void AEnemyBase::ResetWarpTarget()
{

}

TObjectPtr<UArrowComponent> AEnemyBase::GetPositionArrow(EHitDirection HitDirection)
{
	switch (HitDirection)
	{
	case EHitDirection::EHitDirection_Left:
		return LeftArrow;

	case EHitDirection::EHitDirection_Right:
		return RightArrow;

	case EHitDirection::EHitDirection_Front:
		return FrontArrow;

	case EHitDirection::EHitDirection_Back:
		return BackArrow;

	case EHitDirection::EHitDirection_FrontLeft:
		return FrontLeftArrow;

	case EHitDirection::EHitDirection_FrontRight:
		return FrontRightArrow;

	case EHitDirection::EHitDirection_BackLeft:
		return BackLeftArrow;

	case EHitDirection::EHitDirection_BackRight:
		return BackRightArrow;

	default:
		return FrontArrow;
	}
}

