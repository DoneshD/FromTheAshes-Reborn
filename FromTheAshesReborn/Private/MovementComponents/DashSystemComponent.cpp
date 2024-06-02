#include "MovementComponents/DashSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/FromTheAshesRebornGameMode.h"
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

	FTAGameMode = Cast<AFromTheAshesRebornGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!FTAGameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode Cast FAILED"));
	}
}

void UDashSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UDashSystemComponent::GetAngleOfDash()
{
	FVector AttackTargetLocation = FTAGameMode->HardTarget->GetActorLocation();
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

	float TanVal = UKismetMathLibrary::Atan2(TargetDotProduct, DotProduct);
	UE_LOG(LogTemp, Warning, TEXT("TargetDotProduct: %f"), TargetDotProduct);
	UE_LOG(LogTemp, Warning, TEXT("DotProduct: %f"), DotProduct);
	UE_LOG(LogTemp, Warning, TEXT("TanVal: %f "), TanVal);
	UE_LOG(LogTemp, Warning, TEXT("---------------------------------------------"));

	AngleOfDash = UKismetMathLibrary::Atan2(TargetDotProduct, DotProduct) * 180.f / PI;
	UE_LOG(LogTemp, Warning, TEXT("AngleOfDash: %f"), AngleOfDash);
	UE_LOG(LogTemp, Warning, TEXT("---------------------------------------------"));
	return AngleOfDash;
}

bool UDashSystemComponent::InRangeOfLateralDash()
{
	if (GetOwner()->GetDistanceTo(FTAGameMode->HardTarget) < 300.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UDashSystemComponent::SelectBlink()
{
	if (IsDashSaved)
	{
		PerformDash();
		CanDashAttack = true;
	}
}

void UDashSystemComponent::PerformDash()
{
	if (!FTAGameMode->HardTarget)
	{
		if (IsDashSaved)
		{
			CurrentDashAnim = PC->ForwardBlinkAnim;
		}
		else
		{
			CurrentDashAnim = PC->ForwardDashAnim;
		}

		if (CurrentDashAnim)
		{
			PC->PlayAnimMontage(CurrentDashAnim);
		}
		return;
	}

	IPositionalWarpingInterface* TargetPositionalWarpingInterface = Cast<IPositionalWarpingInterface>(FTAGameMode->HardTarget);
	
	float AngleOfDash = GetAngleOfDash();
	float ForwardDashClamp;

	if (InRangeOfLateralDash())
	{
		ForwardDashClamp = 1;
		//EnableLateralDash = true;

	}
	else
	{
		ForwardDashClamp = 25;
		//EnableLateralDash = false;
	}

	if (AngleOfDash >= -ForwardDashClamp && AngleOfDash < ForwardDashClamp)
	{
		CurrentDashAnim = IsDashSaved ? PC->ForwardBlinkAnim : PC->ForwardDashAnim;
	}
	else if (AngleOfDash >= ForwardDashClamp && AngleOfDash <= 135)
	{
		if (InRangeOfLateralDash())
		{
			EnableLateralDash = true;

			if (TargetPositionalWarpingInterface)
			{
				DashWarpTargetArrow = TargetPositionalWarpingInterface->GetRightArrowNeighbor(
					TargetPositionalWarpingInterface->GetPositionalArrow(
						TargetPositionalWarpingInterface->GetFacingDirection(GetOwner()->GetActorLocation())));
			}
		}
		else
		{
			EnableLateralDash = false;
		}
		CurrentDashAnim = IsDashSaved ? PC->RightBlinkAnim : PC->RightDashAnim;
	}
	else if (AngleOfDash >= 135 || AngleOfDash <= -135)
	{
		CurrentDashAnim = IsDashSaved ? PC->BackwardBlinkAnim : PC->BackwardDashAnim;
	}
	else if (AngleOfDash >= -135 && AngleOfDash <= -ForwardDashClamp)
	{
		if (InRangeOfLateralDash())
		{
			EnableLateralDash = true;

			if (TargetPositionalWarpingInterface)
			{
				DashWarpTargetArrow = TargetPositionalWarpingInterface->GetLeftArrowNeighbor(
					TargetPositionalWarpingInterface->GetPositionalArrow(
						TargetPositionalWarpingInterface->GetFacingDirection(GetOwner()->GetActorLocation())));
			}
		}
		else
		{
			EnableLateralDash = false;
		}
		CurrentDashAnim = IsDashSaved ? PC->LeftBlinkAnim : PC->LeftDashAnim;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error"));
		return;
	}

	if (CurrentDashAnim)
	{
		PC->PlayAnimMontage(CurrentDashAnim);
	}
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

	if (FTAGameMode->HardTarget)
	{
		DashTargetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), FTAGameMode->HardTarget->GetActorLocation());
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
	IPositionalWarpingInterface* PositionalWarpingInterface = Cast<IPositionalWarpingInterface>(GetOwner());
	if (PositionalWarpingInterface)
	{
		PositionalWarpingInterface->ResetWarpTargetPostion(FName("DashTarget"));
		DashWarpTargetArrow = NULL;
	}
}

