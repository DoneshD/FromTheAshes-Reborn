#include "MovementComponents/DashSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "MotionWarpingComponent.h"
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

bool UDashSystemComponent::InRangeOfLateralDash()
{
	if (GetOwner()->GetDistanceTo(PC->TargetingSystemComponent->HardTarget) < 300.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UDashSystemComponent::SaveDash()
{
	if (IsDashSaved)
	{
		IPositionalWarpingInterface* TargetPositionalWarpingInterface = Cast<IPositionalWarpingInterface>(PC->TargetingSystemComponent->HardTarget);

		if (TargetPositionalWarpingInterface)
		{
			float AngleOfDash = GetAngleOfDash();

			if (AngleOfDash >= -45 && AngleOfDash < 45)
			{
				PC->PlayAnimMontage(PC->ForwardBlinkAnim);
			}
			else if (AngleOfDash >= 45 && AngleOfDash <= 135)
			{
				if (InRangeOfLateralDash())
				{
					EnableLateralDash = true;

					if (TargetPositionalWarpingInterface)
					{
						DashWarpTargetArrow = TargetPositionalWarpingInterface->GetRightArrowNeighbor(TargetPositionalWarpingInterface->GetPositionalArrow(TargetPositionalWarpingInterface->GetFacingDirection(GetOwner()->GetActorLocation())));
					}
				}
				else
				{
					EnableLateralDash = false;
				}
				PC->PlayAnimMontage(PC->RightBlinkAnim);
			}
			else if (AngleOfDash >= 135 || AngleOfDash <= -135)
			{
				PC->PlayAnimMontage(PC->BackwardBlinkAnim);
			}
			else if (AngleOfDash >= -135 && AngleOfDash <= -45)
			{
				if (InRangeOfLateralDash())
				{
					EnableLateralDash = true;

					if (TargetPositionalWarpingInterface)
					{
						DashWarpTargetArrow = TargetPositionalWarpingInterface->GetLeftArrowNeighbor(TargetPositionalWarpingInterface->GetPositionalArrow(TargetPositionalWarpingInterface->GetFacingDirection(GetOwner()->GetActorLocation())));
					}
				}
				else
				{
					EnableLateralDash = false;
				}
				PC->PlayAnimMontage(PC->LeftBlinkAnim);

			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Error"));
			}
		}
		else
		{
			PC->PlayAnimMontage(PC->ForwardBlinkAnim);
		}

	}
}

void UDashSystemComponent::LockOnDash()
{
	IPositionalWarpingInterface* TargetPositionalWarpingInterface = Cast<IPositionalWarpingInterface>(PC->TargetingSystemComponent->HardTarget);

	float AngleOfDash = GetAngleOfDash();

	if (AngleOfDash >= -45 && AngleOfDash < 45)
	{
		PC->PlayAnimMontage(PC->ForwardDashAnim);
	}
	else if (AngleOfDash >= 45 && AngleOfDash <= 135)
	{
		if (InRangeOfLateralDash())
		{
			EnableLateralDash = true;

			if (TargetPositionalWarpingInterface)
			{
				DashWarpTargetArrow = TargetPositionalWarpingInterface->GetRightArrowNeighbor(TargetPositionalWarpingInterface->GetPositionalArrow(TargetPositionalWarpingInterface->GetFacingDirection(GetOwner()->GetActorLocation())));
			}
		}
		else
		{
			EnableLateralDash = false;
		}
		PC->PlayAnimMontage(PC->RightDashAnim);
	}
	else if (AngleOfDash >= 135 || AngleOfDash <= -135)
	{
		PC->PlayAnimMontage(PC->BackwardDashAnim);
	}
	else if (AngleOfDash >= -135 && AngleOfDash <= -45)
	{
		if (InRangeOfLateralDash())
		{
			EnableLateralDash = true;

			if (TargetPositionalWarpingInterface)
			{
				DashWarpTargetArrow = TargetPositionalWarpingInterface->GetLeftArrowNeighbor(TargetPositionalWarpingInterface->GetPositionalArrow(TargetPositionalWarpingInterface->GetFacingDirection(GetOwner()->GetActorLocation())));
			}
		}
		else
		{
			EnableLateralDash = false;
		}
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

void UDashSystemComponent::DashWarpToTarget()
{
	FMotionWarpingTarget MotionWarpingTargetParams;
	FVector DashTargetLocation;
	FRotator DashTargetRotation;

	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		CharacterMovement = CharacterOwner->GetCharacterMovement();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error getting CharacterMovementComponent"));
	}

	if (EnableLateralDash)
	{
		if (DashWarpTargetArrow)
		{
			DashTargetLocation = DashWarpTargetArrow->GetComponentLocation();
			EnableLateralDash = false;
		}
	}
	else
	{
		if (IsDashSaved)
		{
			IsDashSaved = false;
			DashTargetLocation = GetOwner()->GetActorLocation() + CharacterMovement->GetLastInputVector() * 800.0f;

		}
		else
		{
			DashTargetLocation = GetOwner()->GetActorLocation() + CharacterMovement->GetLastInputVector() * 400.0f;
		}
	}

	DrawDebugSphere(GetWorld(), DashTargetLocation, 10.0, 10, FColor::Red, false, 5.0f);

	if (PC->TargetingSystemComponent->HardTarget)
	{
		DashTargetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), PC->TargetingSystemComponent->HardTarget->GetActorLocation());
	}
	else
	{
		DashTargetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), DashTargetLocation);
	}

	IPositionalWarpingInterface* OwnerPositionalWarpingInterface = Cast<IPositionalWarpingInterface>(GetOwner());

	if (OwnerPositionalWarpingInterface)
	{
		MotionWarpingTargetParams.Name = FName("DashTarget");
		MotionWarpingTargetParams.Location = DashTargetLocation;
		MotionWarpingTargetParams.Rotation.Roll = DashTargetRotation.Roll;
		MotionWarpingTargetParams.Rotation.Yaw = DashTargetRotation.Yaw;
		MotionWarpingTargetParams.BoneName = FName("root");
		
		OwnerPositionalWarpingInterface->UpdateWarpTargetPostion(MotionWarpingTargetParams);
		
	}
}


void UDashSystemComponent::ResetDashWarpToTarget()
{
	AlreadyDashed = true;
	IPositionalWarpingInterface* PositionalWarpingInterface = Cast<IPositionalWarpingInterface>(GetOwner());
	if (PositionalWarpingInterface)
	{
		PositionalWarpingInterface->ResetWarpTargetPostion(FName("DashTarget"));
		DashWarpTargetArrow = NULL;
	}
}

