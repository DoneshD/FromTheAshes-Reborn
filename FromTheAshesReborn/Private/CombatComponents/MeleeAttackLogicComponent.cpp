#include "CombatComponents/MeleeAttackLogicComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "GameModes/FromTheAshesRebornGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "Interfaces/DamagableInterface.h"
#include "Interfaces/AIEnemyInterface.h"
#include "Interfaces/EnemySpawnerInterface.h"
#include "Interfaces/MeleeCombatantInterface.h"
#include "Interfaces/PositionalWarpingInterface.h"
#include "Interfaces/PlayerCharacterInterface.h"
#include "DamageSystem/DamageSystem.h"
#include "Weapons/MeleeWeapon.h"
#include "Enums/EMeleeAttackRange.h"

UMeleeAttackLogicComponent::UMeleeAttackLogicComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UMeleeAttackLogicComponent::BeginPlay()
{
	Super::BeginPlay();

	FTAGameMode = Cast<AFromTheAshesRebornGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!FTAGameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode Cast FAILED"));
	}

}

void UMeleeAttackLogicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsActiveCollision)
	{
		MeleeTraceCollisions();
	}

}

float UMeleeAttackLogicComponent::GetMeleeAttackRange(EMeleeAttackRange AttackRange)
{
	switch (MeleeAttackRange)
	{
	case EMeleeAttackRange::EMeleeAttackRange_Close:
		if (ExtendAttackRange)
		{
			return 800.0f;
		}
		else
		{
			return 600.0f;
		}
	case EMeleeAttackRange::EMeleeAttackRange_Mid:
		if (ExtendAttackRange)
		{
			return 1000.0f;
		}
		else
		{
			return 800.0f;
		}

	case EMeleeAttackRange::EMeleeAttackRange_Far:
		if (ExtendAttackRange)
		{
			return 1200.0f;
		}
		else
		{
			return 1400.0f;
		}

	default:
		return 250.0f;
	}
	return 0.0f;
}

void UMeleeAttackLogicComponent::EmptyHitActorsArray()
{
	AlreadyHitActors_L.Empty();
	AlreadyHitActors_R.Empty();
}

void UMeleeAttackLogicComponent::StartMeleeAttackCollisions()
{
	EmptyHitActorsArray();
	IsActiveCollision = true;
}

void UMeleeAttackLogicComponent::EndMeleeAttackCollisions()
{
	IsActiveCollision = false;
}

bool UMeleeAttackLogicComponent::MeleeWeaponSphereTrace(FVector StartLocation, FVector EndLocation, TArray<FHitResult>& Hits)
{
	TArray<AActor*> ActorArray;
	ActorArray.Add(GetOwner());

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	IAIEnemyInterface* AIEnemy = Cast<IAIEnemyInterface>(GetOwner());
	IPlayerCharacterInterface* PlayerCharacter = Cast<IPlayerCharacterInterface>(GetOwner());
	
	if (AIEnemy)
	{
		
	}
	else if (PlayerCharacter)
	{
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));
	}

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	bool TargetHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		StartLocation,
		EndLocation,
		20.f,
		ObjectTypes,
		false,
		ActorArray,
		EDrawDebugTrace::None,
		Hits,
		true);

	return TargetHit;
}

void UMeleeAttackLogicComponent::MeleeTraceCollisions()
{
	TArray<FHitResult> Hits;
	FVector StartLocation;
	FVector EndLocation;
	UStaticMeshComponent* MeshComponentL;
	UStaticMeshComponent* MeshComponentR;

	IMeleeCombatantInterface* MeleeCombatantInterface = Cast<IMeleeCombatantInterface>(GetOwner());

	if (MeleeCombatantInterface)
	{
		MeshComponentR = MeleeCombatantInterface->GetRightWeapon()->FindComponentByClass<UStaticMeshComponent>();
		if (MeshComponentR)
		{
			StartLocation = MeshComponentR->GetSocketLocation("Start");
			EndLocation = MeshComponentR->GetSocketLocation("End");
		}
	}

	bool bRightSuccess = MeleeWeaponSphereTrace(StartLocation, EndLocation, Hits);

	for (auto& CurrentHit : Hits)
	{
		AActor* HitActor = CurrentHit.GetActor();
		if (HitActor)
		{
			IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(HitActor);
			IEnemySpawnerInterface* EnemySpawnerInterface = Cast<IEnemySpawnerInterface>(HitActor);

			if (EnemySpawnerInterface)
			{
				if (!AlreadyHitActors_R.Contains(HitActor))
				{
					AlreadyHitActors_R.AddUnique(HitActor);

					EnemySpawnerInterface->OnHit();
				}
			}
			if (DamagableInterface)
			{
				if (!AlreadyHitActors_R.Contains(HitActor))
				{
					AlreadyHitActors_R.AddUnique(HitActor);

					IPositionalWarpingInterface* TargetPositionalWarpingInterface = Cast<IPositionalWarpingInterface>(HitActor);
					if (TargetPositionalWarpingInterface)
					{
						EFacingDirection HitReactionDirection = TargetPositionalWarpingInterface->GetFacingDirection(GetOwner()->GetActorLocation());

						FDamageInfo DamageInfo{
							10.0f,                               // DamageAmount
							EDamageType::EDamageType_Melee,      // DamageType
							EDamageResponse::EDamageResponse_HitReaction,  // DamageResponse
							false,                                // ShouldDamageInvincible
							false,                                // CanBeBlocked
							false,                                // CanBeParried
							false,                                // ShouldForceInterrupt
							HitReactionDirection        // HitReactionDirection
						};
						DamagableInterface->TakeDamage(DamageInfo);
					}
				}
			}

		}
	}

	if (HasLeftWeapon)
	{
		if (MeleeCombatantInterface)
		{
			MeshComponentL = MeleeCombatantInterface->GetLeftWeapon()->FindComponentByClass<UStaticMeshComponent>();
			if (MeshComponentL)
			{
				StartLocation = MeshComponentL->GetSocketLocation("Start");
				EndLocation = MeshComponentL->GetSocketLocation("End");
			}
		}
		bool bLeftSuccess = MeleeWeaponSphereTrace(StartLocation, EndLocation, Hits);

		for (auto& CurrentHit : Hits)
		{
			AActor* HitActor = CurrentHit.GetActor();
			if (HitActor)
			{
				IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(HitActor);

				if (DamagableInterface)
				{
					if (!AlreadyHitActors_L.Contains(HitActor))
					{
						AlreadyHitActors_L.AddUnique(HitActor);
						IPositionalWarpingInterface* TargetPositionalWarpingInterface = Cast<IPositionalWarpingInterface>(HitActor);
						if (TargetPositionalWarpingInterface)
						{
							EFacingDirection HitReactionDirection = TargetPositionalWarpingInterface->GetFacingDirection(GetOwner()->GetActorLocation());

							FDamageInfo DamageInfo{
								10.0f,                               // DamageAmount
								EDamageType::EDamageType_Melee,      // DamageType
								EDamageResponse::EDamageResponse_HitReaction,  // DamageResponse
								false,                                // ShouldDamageInvincible
								false,                                // CanBeBlocked
								false,                                // CanBeParried
								false,                                // ShouldForceInterrupt
								HitReactionDirection        // HitReactionDirection
							};

							DamagableInterface->TakeDamage(DamageInfo);

						}
					}
				}
			}
		}
	}
}

void UMeleeAttackLogicComponent::MeleeAttackWarpToTarget(EMeleeAttackRange AttackRange, bool HasInput)
{
	float WarpRange = GetMeleeAttackRange(AttackRange);

	FMotionWarpingTarget MotionWarpingTargetParams;

	FVector StartLocation;
	FVector EndLocation;

	FVector WarpTargetLocation;
	FRotator TargetRotation;

	if (AActor* OwnerActor = GetOwner())
	{
		if (FTAGameMode->HardTarget)
		{
			StartLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 200;
			EndLocation = OwnerActor->GetActorLocation() + OwnerActor->GetActorForwardVector() * WarpRange;
		}
		else
		{
			if (HasInput)
			{
				if (ACharacter* CharacterOwner = Cast<ACharacter>(OwnerActor))
				{
					CharacterMovement = CharacterOwner->GetCharacterMovement();
					StartLocation = GetOwner()->GetActorLocation() + CharacterMovement->GetLastInputVector() * 100;
					EndLocation = GetOwner()->GetActorLocation() + CharacterMovement->GetLastInputVector() * WarpRange;
				}
			}
			else
			{
				StartLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 200;
				EndLocation = OwnerActor->GetActorLocation() + OwnerActor->GetActorForwardVector() * WarpRange;
			}
		}
	}

	FHitResult OutHit;

	TArray<AActor*> ActorArray;
	ActorArray.Add(GetOwner());

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	bool TargetHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		StartLocation,
		EndLocation,
		75.f,
		ObjectTypes,
		false,
		ActorArray,
		EDrawDebugTrace::None ,
		OutHit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		1.0f
	);

	if (TargetHit)
	{
		AActor* HitActor = OutHit.GetActor();
		if (HitActor)
		{
			IPositionalWarpingInterface* PositionalWarpingInterface = Cast<IPositionalWarpingInterface>(HitActor);
			if (PositionalWarpingInterface)
			{
				EFacingDirection FacingDirection = PositionalWarpingInterface->GetFacingDirection(GetOwner()->GetActorLocation());
				WarpTargetLocation = PositionalWarpingInterface->GetPositionalArrow(FacingDirection)->GetComponentLocation();

				FVector TargetLocation = HitActor->GetActorLocation() - HitActor->GetActorForwardVector();
				TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), TargetLocation);

			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("NOT PositionalWarpingInterface"));
				WarpTargetLocation = HitActor->GetActorLocation();
				TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), WarpTargetLocation);
			}
		}
	}
	else
	{
		if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
		{
			CharacterMovement = CharacterOwner->GetCharacterMovement();
			if (HasInput)
			{
				WarpTargetLocation = GetOwner()->GetActorLocation() + CharacterMovement->GetLastInputVector() * (WarpRange - 400.0f);
			}
			else
			{
				WarpTargetLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * (WarpRange - 400.0f);
			}
		}

		//DrawDebugSphere(GetWorld(), WarpTargetLocation, 10.0, 10, FColor::Red, false, 5.0f);
	}

	TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), WarpTargetLocation);

	MotionWarpingTargetParams.Name = FName("CombatTarget");
	MotionWarpingTargetParams.Location = WarpTargetLocation;
	MotionWarpingTargetParams.Rotation.Roll = TargetRotation.Roll;
	MotionWarpingTargetParams.Rotation.Yaw = TargetRotation.Yaw;
	MotionWarpingTargetParams.BoneName = FName("root");

	IPositionalWarpingInterface* OwnerPositionalWarpingInterface = Cast<IPositionalWarpingInterface>(GetOwner());

	if (OwnerPositionalWarpingInterface)
	{
		OwnerPositionalWarpingInterface->UpdateWarpTargetPostion(MotionWarpingTargetParams);
	}
}

void UMeleeAttackLogicComponent::ResetMeleeAttackWarpToTarget()
{
	ExtendAttackRange = false;

	IPositionalWarpingInterface* PositionalWarpingInterface = Cast<IPositionalWarpingInterface>(GetOwner());
	if (PositionalWarpingInterface)
	{
		PositionalWarpingInterface->ResetWarpTargetPostion(FName("CombatTarget"));
	}
}