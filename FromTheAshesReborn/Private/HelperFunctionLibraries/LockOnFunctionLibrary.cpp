#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


FLockOnAngleResult ULockOnFunctionLibrary::AngleFromInputVectorToLockedTarget(AActor* OwningActor, AActor* LockOnTarget)
{
	FLockOnAngleResult Result;
	
	FVector AttackTargetLocation = LockOnTarget->GetActorLocation();
	FVector OwnerLocation = OwningActor->GetActorLocation();
	UCharacterMovementComponent* CMC = nullptr;

	if (ACharacter* CharacterOwner = Cast<ACharacter>(OwningActor))
	{
		CMC = CharacterOwner->GetCharacterMovement();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error getting CharacterMovementComponent"));
		return FLockOnAngleResult();
	}
	Result.InputVector = CMC->GetLastInputVector();
	
	FVector MovementVectorAnchored = CMC->GetLastInputVector() + OwningActor->GetActorLocation();

	FVector TargetDifference = AttackTargetLocation - OwnerLocation;
	FVector TargetVector = FVector(TargetDifference.X, TargetDifference.Y, 0.0f);

	FVector MovementDifference = MovementVectorAnchored - OwnerLocation;
	FVector MovementVector = FVector(MovementDifference.X, MovementDifference.Y, 0.0f);

	FVector CrossProduct = FVector::CrossProduct(TargetVector, MovementVector);

	float TargetDotProduct = FVector::DotProduct(CrossProduct, FVector(0.0f, 0.0f, 1.0f));
	float DotProduct = FVector::DotProduct(TargetVector, MovementVector);

	Result.Angle = UKismetMathLibrary::Atan2(TargetDotProduct, DotProduct) * 180.f / PI;
	
	return Result;
}

ELockOnInputOrientationDirection ULockOnFunctionLibrary::GetOrientationOfInput(FLockOnAngleResult AngleResult)
{
	if (AngleResult.Angle >= -25.0f && AngleResult.Angle < 25.0f && !AngleResult.InputVector.IsNearlyZero())
	{
		return ELockOnInputOrientationDirection::Forward;
	}
	if (AngleResult.Angle > 25.0f && AngleResult.Angle < 135.0f)
	{
		return ELockOnInputOrientationDirection::Right;
	}
	if (AngleResult.Angle < -25.0f && AngleResult.Angle > -135.0f)
	{
		return ELockOnInputOrientationDirection::Left;
	}
	if (FMath::Abs(AngleResult.Angle) >= 135.0f)
	{
		return ELockOnInputOrientationDirection::Backward;
	}

	UE_LOG(LogTemp, Error, TEXT("OrientationOfInput None"));
	return ELockOnInputOrientationDirection::None;
}

FRotator ULockOnFunctionLibrary::CheckRotationBasedOnTarget(AActor* OwningActor, AActor* LockOnTarget, FVector Location)
{
	if (LockOnTarget)
	{
		return UKismetMathLibrary::FindLookAtRotation(OwningActor->GetActorLocation(), LockOnTarget->GetActorLocation());
	}
	
	return UKismetMathLibrary::FindLookAtRotation(OwningActor->GetActorLocation(), Location);
}
