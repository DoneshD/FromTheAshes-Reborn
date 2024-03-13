#include "DashSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "MotionWarpingComponent.h"
#include "Interfaces/MotionWarpingInterface.h"


UDashSystemComponent::UDashSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UDashSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UDashSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UDashSystemComponent::StartDash()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Dash"));
	
	
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

