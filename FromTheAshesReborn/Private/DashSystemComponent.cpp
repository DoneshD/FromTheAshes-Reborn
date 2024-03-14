#include "DashSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "MotionWarpingComponent.h"
#include "Interfaces/MotionWarpingInterface.h"
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

void UDashSystemComponent::LockOnDash()
{
	
	FVector AttackTargetLocation = PC->TargetingSystemComponent->HardTarget->GetActorLocation();
	FVector OwnerLocation = PC->GetActorLocation();
	FVector MovementVectorAnchored = PC->GetCharacterMovement()->GetLastInputVector() + PC->GetActorLocation();

	float AngleOfDodge = 0.0f;

	FVector TargetDifference = AttackTargetLocation - OwnerLocation;
	FVector TargetVector = FVector(TargetDifference.X, TargetDifference.Y, 0.0f);

	FVector MovementDifference = MovementVectorAnchored - OwnerLocation;
	FVector MovementVector = FVector(MovementDifference.X, MovementDifference.Y, 0.0f);

	FVector CrossProduct = FVector::CrossProduct(TargetVector, MovementVector);

	float TargetDotProduct = FVector::DotProduct(CrossProduct, FVector(0.0f, 0.0f, 1.0f));
	float DotProduct = FVector::DotProduct(TargetVector, MovementVector);

	AngleOfDodge = UKismetMathLibrary::Atan2(TargetDotProduct, DotProduct) * 180.f / PI;

	if (AngleOfDodge >= -45 && AngleOfDodge < 45)
	{
		PC->PlayAnimMontage(PC->ForwardDashAnim);
	}
	else if (AngleOfDodge >= 45 && AngleOfDodge <= 135)
	{
		PC->PlayAnimMontage(PC->RightDashAnim);
	}

	else if (AngleOfDodge >= 135 || AngleOfDodge <= -135)
	{
		PC->PlayAnimMontage(PC->BackwardDashAnim);
	}
	else if (AngleOfDodge >= -135 && AngleOfDodge <= -45)
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
		UE_LOG(LogTemp, Warning, TEXT("End Location: %s"), *TargetLocation.ToString());
		DrawDebugSphere(GetWorld(), TargetLocation, 10.0, 10, FColor::Red, false, 5.0f);

		AActor* OwnerActor = GetOwner();
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), TargetLocation);


		IMotionWarpingInterface* MotionWarpingInterface = Cast<IMotionWarpingInterface>(OwnerActor);
		if (MotionWarpingInterface)
		{
			UMotionWarpingComponent* MotionWarpingComponent = GetOwner()->FindComponentByClass<UMotionWarpingComponent>();
			if (MotionWarpingComponent)
			{

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

