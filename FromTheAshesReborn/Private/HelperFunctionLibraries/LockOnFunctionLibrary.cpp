#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


float ULockOnFunctionLibrary::AngleFromInputVectorToLockedTarget(AActor* OwningActor, AActor* LockOnTarget)
{
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
		return 0.0f;
	}
	FVector MovementVectorAnchored = CMC->GetLastInputVector() + OwningActor->GetActorLocation();

	float AngleFromTarget = 0.0f;

	FVector TargetDifference = AttackTargetLocation - OwnerLocation;
	FVector TargetVector = FVector(TargetDifference.X, TargetDifference.Y, 0.0f);

	FVector MovementDifference = MovementVectorAnchored - OwnerLocation;
	FVector MovementVector = FVector(MovementDifference.X, MovementDifference.Y, 0.0f);

	FVector CrossProduct = FVector::CrossProduct(TargetVector, MovementVector);

	float TargetDotProduct = FVector::DotProduct(CrossProduct, FVector(0.0f, 0.0f, 1.0f));
	float DotProduct = FVector::DotProduct(TargetVector, MovementVector);

	float TanVal = UKismetMathLibrary::Atan2(TargetDotProduct, DotProduct);

	AngleFromTarget = UKismetMathLibrary::Atan2(TargetDotProduct, DotProduct) * 180.f / PI;
	return AngleFromTarget;
}

ELockOnInputOrientationDirection ULockOnFunctionLibrary::OrientationOfInput(float Angle)
{
	if (Angle >= -25.0f && Angle < 25.0f)
	{
		return ELockOnInputOrientationDirection::Forward;
	}
	if (Angle > 25.0f && Angle < 135.0f)
	{
		return ELockOnInputOrientationDirection::Right;
	}
	if (Angle < -25.0f && Angle > -135.0f)
	{
		return ELockOnInputOrientationDirection::Left;
	}
	if (FMath::Abs(Angle) >= 135.0f)
	{
		return ELockOnInputOrientationDirection::Backward;
	}

	UE_LOG(LogTemp, Error, TEXT("OrientationOfInput None"));
	return ELockOnInputOrientationDirection::None;
}