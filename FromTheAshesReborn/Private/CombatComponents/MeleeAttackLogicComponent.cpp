#include "CombatComponents/MeleeAttackLogicComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DamageSystem/DamageSystem.h"
#include "MotionWarpingComponent.h"
#include "Interfaces/DamagableInterface.h"
#include "Interfaces/MotionWarpingInterface.h"
#include "Interfaces/AIEnemyInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CombatComponents/ComboSystemComponent.h"
#include "Components/ArrowComponent.h"
#include "Enums/EMeleeAttackRange.h"

#include "Kismet/KismetMathLibrary.h"

UMeleeAttackLogicComponent::UMeleeAttackLogicComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UMeleeAttackLogicComponent::BeginPlay()
{
	Super::BeginPlay();
	
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
		return 250.0f;

	case EMeleeAttackRange::EMeleeAttackRange_Mid:
		return 400.0f;

	case EMeleeAttackRange::EMeleeAttackRange_Far:
		return 750.0f;

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
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

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

	USkeletalMeshComponent* MeshComponent = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	if (MeshComponent)
	{
		StartLocation = MeshComponent->GetSocketLocation("Start_L");
		EndLocation = MeshComponent->GetSocketLocation("End_L");
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
					FDamageInfo DamageInfo{
						20.0f,                               // DamageAmount
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

	StartLocation = MeshComponent->GetSocketLocation("Start_R");
	EndLocation = MeshComponent->GetSocketLocation("End_R");

	bool bRightSuccess = MeleeWeaponSphereTrace(StartLocation, EndLocation, Hits);

	for (auto& CurrentHit : Hits)
	{
		AActor* HitActor = CurrentHit.GetActor();
		if (HitActor)
		{
			IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(HitActor);

			if (DamagableInterface)
			{

				if (!AlreadyHitActors_R.Contains(HitActor))
				{
					AlreadyHitActors_R.AddUnique(HitActor);
					FDamageInfo DamageInfo{
							20.0f,                            // DamageAmount
							EDamageType::EDamageType_Melee,   // DamageType
							EDamageResponse::EDamageResponse_HitReaction,  // DamageResponse
							false,                            // ShouldDamageInvincible
							false,                            // CanBeBlocked
							false,                            // CanBeParried
							false,                             // ShouldForceInterrupt
							HitReactionDirection
					};
					DamagableInterface->TakeDamage(DamageInfo);
				}
			}
		}
	}
}

void UMeleeAttackLogicComponent::MeleeAttackWarpToTarget(FMotionWarpingTarget& MotionWarpingTargetParams, EMeleeAttackRange AttackRange, bool HasInput)
{
	
	float WarpRange = GetMeleeAttackRange(AttackRange);
	FVector EndLocation;
	if (AActor* OwnerActor = GetOwner())
	{
		if (HasInput)
		{
			if (ACharacter* CharacterOwner = Cast<ACharacter>(OwnerActor))
			{
				UCharacterMovementComponent* CharacterMovement = CharacterOwner->GetCharacterMovement();
				EndLocation = GetOwner()->GetActorLocation() + CharacterMovement->GetLastInputVector() * WarpRange;
			}
		}
		else
		{
			EndLocation = OwnerActor->GetActorLocation() + OwnerActor->GetActorForwardVector() * WarpRange;
		}
	}

	FHitResult OutHit;

	TArray<AActor*> ActorArray;
	ActorArray.Add(GetOwner());

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	bool TargetHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector(),
		EndLocation,
		75.f,
		ObjectTypes,
		false,
		ActorArray,
		EDrawDebugTrace::ForDuration,
		OutHit,
		true
	);

	if (TargetHit)
	{
		AActor* HitActor = OutHit.GetActor();
		if (HitActor)
		{
			IMotionWarpingInterface* MotionWarpingInterface = Cast<IMotionWarpingInterface>(HitActor);
			if(MotionWarpingInterface)
			{
				EHitDirection HitDirection = MotionWarpingInterface->GetHitEnemyDirection(GetOwner()->GetActorLocation());
				UMotionWarpingComponent* MotionWarpingComponent = GetOwner()->FindComponentByClass<UMotionWarpingComponent>();
				if (MotionWarpingComponent)
				{
					WarpTargetArrow = MotionWarpingInterface->GetPositionArrow(HitDirection);
					FVector WarpTargetLocation = WarpTargetArrow->GetComponentLocation();

					FVector TargetLocation = HitActor->GetActorLocation() - HitActor->GetActorForwardVector();
					FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), TargetLocation);

					MotionWarpingTargetParams.Name = FName("CombatTarget");
					MotionWarpingTargetParams.Location = WarpTargetLocation;
					MotionWarpingTargetParams.Rotation.Roll = TargetRotation.Roll;
					MotionWarpingTargetParams.Rotation.Yaw = TargetRotation.Yaw;
					MotionWarpingTargetParams.BoneName = FName("root");

					MotionWarpingComponent->AddOrUpdateWarpTarget(MotionWarpingTargetParams);
				}
			}
		}
	}
	
}

void UMeleeAttackLogicComponent::ResetMeleeAttackWarpToTarget()
{
	
	IMotionWarpingInterface* MotionWarpingInterface = Cast<IMotionWarpingInterface>(GetOwner());
	if (MotionWarpingInterface)
	{
		UMotionWarpingComponent* MotionWarpingComponent = GetOwner()->FindComponentByClass<UMotionWarpingComponent>();
		if (MotionWarpingComponent)
		{
			MotionWarpingComponent->RemoveWarpTarget(FName("CombatTarget"));
			WarpTargetArrow = NULL;
		}
	}
}