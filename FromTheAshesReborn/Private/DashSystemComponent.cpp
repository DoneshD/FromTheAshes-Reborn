#include "DashSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "MotionWarpingComponent.h"
#include "Interfaces/MotionWarpingInterface.h"
#include "Components/ArrowComponent.h"
#include "Characters/EnemyBase.h"
#include "Characters/PlayableCharacter.h"

UDashSystemComponent::UDashSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UDashSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	PC = Cast<APlayableCharacter>(GetOwner());

}

void UDashSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UDashSystemComponent::GetAngleOfDash()
{
	FVector AttackTargetLocation = PC->TargetingSystemComponent->HardTarget->GetActorLocation();
	FVector OwnerLocation = PC->GetActorLocation();
	FVector MovementVectorAnchored = PC->GetCharacterMovement()->GetLastInputVector() + PC->GetActorLocation();

	float AngleOfDash = 0.0f;

	FVector TargetDifference = AttackTargetLocation - OwnerLocation;
	FVector TargetVector = FVector(TargetDifference.X, TargetDifference.Y, 0.0f);

	FVector MovementDifference = MovementVectorAnchored - OwnerLocation;
	FVector MovementVector = FVector(MovementDifference.X, MovementDifference.Y, 0.0f);

	FVector CrossProduct = FVector::CrossProduct(TargetVector, MovementVector);

	float TargetDotProduct = FVector::DotProduct(CrossProduct, FVector(0.0f, 0.0f, 1.0f));
	float DotProduct = FVector::DotProduct(TargetVector, MovementVector);

	AngleOfDash = UKismetMathLibrary::Atan2(TargetDotProduct, DotProduct) * 180.f / PI;

	return AngleOfDash;
}

void UDashSystemComponent::LockOnDash()
{
	
	float AngleOfDash = GetAngleOfDash();

	if (AngleOfDash >= -45 && AngleOfDash < 45)
	{
		PC->PlayAnimMontage(PC->ForwardDashAnim);
	}
	else if (AngleOfDash >= 45 && AngleOfDash <= 135)
	{
		PC->PlayAnimMontage(PC->RightDashAnim);
	}
	else if (AngleOfDash >= 135 || AngleOfDash <= -135)
	{
		PC->PlayAnimMontage(PC->BackwardDashAnim);
	}
	else if (AngleOfDash >= -135 && AngleOfDash <= -45)
	{
		PC->PlayAnimMontage(PC->LeftDashAnim);
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error"));
	}
}

void UDashSystemComponent::FreeLockDash()
{
	PC->PlayAnimMontage(PC->ForwardDashAnim);
}

void UDashSystemComponent::DashWarpToTarget(FMotionWarpingTarget& MotionWarpingTargetParams)
{
	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		UCharacterMovementComponent* CharacterMovement = CharacterOwner->GetCharacterMovement();
		FVector TargetLocation = GetOwner()->GetActorLocation() + CharacterMovement->GetLastInputVector() * 400.0f;
		DrawDebugSphere(GetWorld(), TargetLocation, 10.0, 10, FColor::Red, false, 5.0f);

		FRotator TargetRotation;

		if (PC->TargetingSystemComponent->HardTarget)
		{
			TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), PC->TargetingSystemComponent->HardTarget->GetActorLocation());
		}
		else
		{
			TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), TargetLocation);
		}

		IMotionWarpingInterface* MotionWarpingInterface = Cast<IMotionWarpingInterface>(PC->TargetingSystemComponent->HardTarget);

		if (MotionWarpingInterface)
		{
			EHitDirection HitDirection = MotionWarpingInterface->GetHitEnemyDirection(GetOwner()->GetActorLocation());
			DashWarpTargetArrow = MotionWarpingInterface->GetPositionArrow(HitDirection);

			UMotionWarpingComponent* MotionWarpingComponent = GetOwner()->FindComponentByClass<UMotionWarpingComponent>();
			if (MotionWarpingComponent)
			{
				UE_LOG(LogTemp, Warning, TEXT("Warping target"));
				MotionWarpingTargetParams.Name = FName("DashTarget");
				MotionWarpingTargetParams.Location = TargetLocation;
				MotionWarpingTargetParams.Rotation.Roll = TargetRotation.Roll;
				MotionWarpingTargetParams.Rotation.Yaw = TargetRotation.Yaw;
				MotionWarpingTargetParams.BoneName = FName("root");

				MotionWarpingComponent->AddOrUpdateWarpTarget(MotionWarpingTargetParams);
			}
		}
	}
}

void UDashSystemComponent::ResetDashWarpToTarget()
{
	IMotionWarpingInterface* MotionWarpingInterface = Cast<IMotionWarpingInterface>(GetOwner());
	if (MotionWarpingInterface)
	{
		UMotionWarpingComponent* MotionWarpingComponent = GetOwner()->FindComponentByClass<UMotionWarpingComponent>();
		if (MotionWarpingComponent)
		{
			MotionWarpingComponent->RemoveWarpTarget(FName("DashTarget"));
		}
	}
}

