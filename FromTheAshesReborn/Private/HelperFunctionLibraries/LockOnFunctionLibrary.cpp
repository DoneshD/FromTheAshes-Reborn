#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


float ULockOnFunctionLibrary::AngleFromLockedTarget(AActor* OwningActor, AActor* LockOnTarget)
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
