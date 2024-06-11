#include "CombatComponents/AerialSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "Interfaces/PositionalWarpingInterface.h"

UAerialSystemComponent::UAerialSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UAerialSystemComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UAerialSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UAerialSystemComponent::JumpLineTrace(FVector StartLocation, FVector EndLocation)
{
    DrawDebugLine(
        GetWorld(),
        StartLocation,
        EndLocation,
        FColor::Red,
        false,  // Persistent lines
        1.0f,   // Lifetime of the line
        0,      // Depth priority
        5.0f    // Line thickness
    );
    UE_LOG(LogTemp, Warning, TEXT("Drake"));
	return false;
}

void UAerialSystemComponent::JumpWarpToTarget()
{
    FMotionWarpingTarget MotionWarpingTargetParams;
    FVector StartLocation = GetOwner()->GetActorLocation();
    FVector EndLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorUpVector() * 300.0f;
    JumpLineTrace(StartLocation, EndLocation);

    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(
        GetOwner()->GetActorLocation(), StartLocation);


    IPositionalWarpingInterface* OwnerPositionalWarpingInterface = Cast<IPositionalWarpingInterface>(GetOwner());

    if (OwnerPositionalWarpingInterface)
    {
        MotionWarpingTargetParams.Name = FName("JumpTarget");
        MotionWarpingTargetParams.Location = EndLocation;
        MotionWarpingTargetParams.Rotation.Roll = TargetRotation.Roll;
        MotionWarpingTargetParams.Rotation.Yaw = TargetRotation.Yaw;
        MotionWarpingTargetParams.BoneName = FName("root");

        OwnerPositionalWarpingInterface->UpdateWarpTargetPostion(MotionWarpingTargetParams);
    }
}

void UAerialSystemComponent::ResetJumpWarpToTarget()
{
}
