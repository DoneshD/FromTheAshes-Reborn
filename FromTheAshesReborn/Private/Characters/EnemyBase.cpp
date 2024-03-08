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

	//Bind Death Event
	//Bind Hit Response Event
	DamageSystemComponent->OnDamageResponse.AddUObject(this, &AEnemyBase::HandleHitReaction);

}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//not good!!!!!!!
	if (AICEnemyBase->AttackTarget)
	{
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AICEnemyBase->AttackTarget->GetActorLocation()));
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
		DamagableInterface->ReturnAttackToken(TokensUsedInCurrentAttack);
		StoreAttackTokens(AttackTarget, -1 * TokensUsedInCurrentAttack);
	}
}

void AEnemyBase::FinishLightMeleeAttack()
{
	OnAttackEnd.Execute();
}

void AEnemyBase::HandleHitReaction(FDamageInfo DamageInfo)
{
	GetCharacterMovement()->StopMovementImmediately();
	AICEnemyBase->SetStateAsFrozen();
	
	UAnimMontage* HitReactionMontage = FrontHitReaction;

	switch (DamageInfo.HitReactionDirection)
	{
	case EHitReactionDirection::EHitReactionDirection_Left:
		HitReactionMontage = LeftHitReaction;
		break;

	case EHitReactionDirection::EHitReactionDirection_Right:
		HitReactionMontage = RightHitReaction;
		break;

	case EHitReactionDirection::EHitReactionDirection_Front:
		HitReactionMontage = FrontHitReaction;
		break;

	case EHitReactionDirection::EHitReactionDirection_Back:
		HitReactionMontage = BackHitReaction;
		break;


	//case EHitReactionDirection::EHitReactionDirection_None:
		//HitReactionMontage = KnockBackHitReaction;
		//break;
	}
	if (HitReactionMontage)
	{
		PlayAnimMontage(HitReactionMontage);

		FOnMontageEnded CompleteDelegate;
		CompleteDelegate.BindUObject(this, &AEnemyBase::OnMontageCompleted);
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(CompleteDelegate, HitReactionMontage);

		FOnMontageEnded BlendOutDelegate;
		BlendOutDelegate.BindUObject(this, &AEnemyBase::OnMontageInterrupted);
		GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendOutDelegate, HitReactionMontage);
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
	if (ReservedAttackTokensMap.Find(AttackTarget))
	{
		Amount += ReservedAttackTokensMap[AttackTarget];
	}
	ReservedAttackTokensMap.Add(AttackTarget, Amount);
}

void AEnemyBase::OnTargeted()
{
	GetMesh()->SetOverlayMaterial(TargetedMaterial);
}

void AEnemyBase::EndTargeted()
{
	UMaterialInterface* DefaultMaterial = GetMesh()->GetMaterial(1);
	GetMesh()->SetOverlayMaterial(DefaultMaterial);
}

EHitReactionDirection AEnemyBase::GetHitEnemyDirection(FVector HitLocation)
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
		return EHitReactionDirection::EHitReactionDirection_Left;

	case 1:
		return EHitReactionDirection::EHitReactionDirection_Right;

	case 2:
		return EHitReactionDirection::EHitReactionDirection_Front;

	case 3:
		return EHitReactionDirection::EHitReactionDirection_Back;

	default:
		break;
	}
	return EHitReactionDirection::EHitReactionDirection_None;
}

void AEnemyBase::UpdateWarpTarget(FMotionWarpingTarget& MotionWarpingTargetParams)
{
}

void AEnemyBase::ResetWarpTarget()
{
}

TObjectPtr<UArrowComponent> AEnemyBase::GetPositionArrow(EHitReactionDirection HitDirection)
{
	switch (HitDirection)
	{
	case EHitReactionDirection::EHitReactionDirection_Left:
		return LeftArrow;

	case EHitReactionDirection::EHitReactionDirection_Back:
		return BackArrow;

	case EHitReactionDirection::EHitReactionDirection_Front:
		return FrontArrow;

	case EHitReactionDirection::EHitReactionDirection_Right:
		return RightArrow;

	default:
		return FrontArrow;
	}
}

