#include "WallRunningComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UWallRunningComponent::UWallRunningComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UWallRunningComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UWallRunningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UWallRunningComponent::CanWallRun()
{
	ACharacter* CharRef = Cast<ACharacter>(GetOwner());
	bool IsFalling = CharRef->GetCharacterMovement()->IsFalling();
	bool IsHoldingForward = CharRef->GetCharacterMovement()->GetLastInputVector().X > 0;

	return (bWallLeftHit || bWallRightHit) && IsFalling && IsHoldingForward;
}

void UWallRunningComponent::CheckForWall()
{
	FVector StartLocation = GetOwner()->GetActorLocation();
	FVector EndLocationLeft = GetOwner()->GetActorLocation() + GetOwner()->GetActorRightVector() * (WallRunCheckDistance * -1);
	FVector EndLocationRight = GetOwner()->GetActorLocation() + GetOwner()->GetActorRightVector() * WallRunCheckDistance;

	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	QueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	bWallLeftHit = GetWorld()->LineTraceSingleByObjectType(WallRightHitResult, StartLocation, EndLocationLeft, QueryParams);
	bWallRightHit = GetWorld()->LineTraceSingleByObjectType(WallRightHitResult, StartLocation, EndLocationRight, QueryParams);
	
}

void UWallRunningComponent::CheckWallRunning()
{
	if(CanWallRun())
	{
		if(IsWallRunning)
		{
			IsWallRunning = false;
		}
		else if (!IsWallRunning)
		{
			IsWallRunning = true;
		}
		ACharacter* CharRef = Cast<ACharacter>(GetOwner());
		CharRef->GetCharacterMovement()->Velocity = FVector(CharRef->GetVelocity().X, CharRef->GetVelocity().Y, FMath::Clamp(CharRef->GetVelocity().Z, -25.0, 35.0f));
		CharRef->GetCharacterMovement()->GravityScale = 0;
	}
}

void UWallRunningComponent::WallRunningMovement()
{
	
}

