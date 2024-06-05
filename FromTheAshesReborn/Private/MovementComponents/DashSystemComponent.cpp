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
	FVector OwnerLocation = GetOwner()->GetActorLocation();

	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		CharacterMovement = CharacterOwner->GetCharacterMovement();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error getting CharacterMovementComponent"));
		return 0.0f;
	}
	FVector MovementVectorAnchored = CharacterMovement->GetLastInputVector() + GetOwner()->GetActorLocation();

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
		IPlayerCharacterInterface* PlayerCharacterInterface = Cast<IPlayerCharacterInterface>(GetOwner());
		PerformDash(PlayerCharacterInterface->GetHasMovementInput());
		CanDashAttack = true;
	}
}

void UDashSystemComponent::PerformDash(bool HasInput)
{
	IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(GetOwner());
	if (AIEnemyInterface)
	{
		UE_LOG(LogTemp, Warning, TEXT("PerformDash component called by AI enemy"));
		return;
	}

	if (!FTAGameMode->HardTarget)
	{
		CurrentDashAnim = IsDashSaved ? ForwardBlinkAnim : ForwardDashAnim;
		if (!CurrentDashAnim)
		{
			return;
		}

		if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
		{
			CharacterOwner->PlayAnimMontage(CurrentDashAnim);
		}
		else
		{
			return;
		}
		
	}

	IPositionalWarpingInterface* TargetPositionalWarpingInterface = Cast<IPositionalWarpingInterface>(FTAGameMode->HardTarget);
	float AngleOfDash = GetAngleOfDash();
	float ForwardDashClamp = InRangeOfLateralDash() ? 1 : 25;

	if (InRangeOfLateralDash() && !HasInput)
	{
		CurrentDashAnim = FMath::RandBool() ? LeftDashAnim : RightDashAnim;
		if (!CurrentDashAnim)
		{
			return;
		}

		if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
		{
			CharacterOwner->PlayAnimMontage(CurrentDashAnim);
		}
		else
		{
			return;
		}
	}

	if (AngleOfDash >= -ForwardDashClamp && AngleOfDash < ForwardDashClamp)
	{
		CurrentDashAnim = IsDashSaved ? ForwardBlinkAnim : ForwardDashAnim;
	}
	else if (AngleOfDash >= ForwardDashClamp && AngleOfDash <= 135)
	{
		EnableLateralDash = InRangeOfLateralDash();
		if (EnableLateralDash && TargetPositionalWarpingInterface)
		{
			DashWarpTargetArrow = TargetPositionalWarpingInterface->GetRightArrowNeighbor(
				TargetPositionalWarpingInterface->GetPositionalArrow(
					TargetPositionalWarpingInterface->GetFacingDirection(GetOwner()->GetActorLocation())));
		}
		CurrentDashAnim = IsDashSaved ? RightBlinkAnim : RightDashAnim;
	}
	else if (AngleOfDash >= 135 || AngleOfDash <= -135)
	{
		CurrentDashAnim = IsDashSaved ? BackwardBlinkAnim : BackwardDashAnim;
	}
	else if (AngleOfDash >= -135 && AngleOfDash <= -ForwardDashClamp)
	{
		EnableLateralDash = InRangeOfLateralDash();
		if (EnableLateralDash && TargetPositionalWarpingInterface)
		{
			DashWarpTargetArrow = TargetPositionalWarpingInterface->GetLeftArrowNeighbor(
				TargetPositionalWarpingInterface->GetPositionalArrow(
					TargetPositionalWarpingInterface->GetFacingDirection(GetOwner()->GetActorLocation())));
		}
		CurrentDashAnim = IsDashSaved ? LeftBlinkAnim : LeftDashAnim;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error"));
		return;
	}

	if (!CurrentDashAnim)
	{
		return;
	}

	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		CharacterOwner->PlayAnimMontage(CurrentDashAnim);
	}
	else
	{
		return;
	}
}


void UDashSystemComponent::DashWarpToTarget(bool HasInput)
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
		FVector DashDirection;
		float DashDistance = 400.0f;

		if (IsDashSaved)
		{
			IsDashSaved = false;
			DashDistance = 1000.0f;
		}

		if (HasInput)
		{
			DashDirection = CharacterMovement->GetLastInputVector();
		}
		else
		{
			DashDirection = GetOwner()->GetActorForwardVector();
		}

		DashTargetLocation = GetOwner()->GetActorLocation() + DashDistance * DashDirection;

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
