#include "WallRunningComponent.h"

#include "FrameTypes.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UWallRunningComponent::UWallRunningComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UWallRunningComponent::BeginPlay()
{
	Super::BeginPlay();
	CharRef = Cast<ACharacter>(GetOwner());
	
}

void UWallRunningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CheckForWall();
	CheckWallRunning();
	WallRunningMovement();

}

bool UWallRunningComponent::CanWallRun()
{
	bool IsFalling = CharRef->GetCharacterMovement()->IsFalling();
	
	//Check later
	// bool IsHoldingForward = CharRef->GetCharacterMovement()->GetLastInputVector().X > 0;
	
	APlayerController* PC = Cast<APlayerController>(CharRef->GetController());
	bool IsHoldingForward = PC && PC->IsInputKeyDown(EKeys::W);

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

	bWallLeftHit = GetWorld()->LineTraceSingleByObjectType(WallLeftHitResult, StartLocation, EndLocationLeft, QueryParams);
	bWallRightHit = GetWorld()->LineTraceSingleByObjectType(WallRightHitResult, StartLocation, EndLocationRight, QueryParams);

	// Debug Draw Left Trace (Red for no hit, Green if hit)
	FColor LeftColor = bWallLeftHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), StartLocation, EndLocationLeft, LeftColor, false, 2.0f, 0, 2.0f);
	
	// Debug Draw Right Trace (Red for no hit, Blue if hit)
	FColor RightColor = bWallRightHit ? FColor::Blue : FColor::Red;
	DrawDebugLine(GetWorld(), StartLocation, EndLocationRight, RightColor, false, 2.0f, 0, 2.0f);
	
	// Draw Impact Points if hit
	if (bWallLeftHit)
	{
		DrawDebugPoint(GetWorld(), WallLeftHitResult.ImpactPoint, 10.0f, FColor::Green, false, 2.0f);
	}
	
	if (bWallRightHit)
	{
		DrawDebugPoint(GetWorld(), WallRightHitResult.ImpactPoint, 10.0f, FColor::Blue, false, 2.0f);
	}
	
}

void UWallRunningComponent::CheckWallRunning()
{
	if(CanWallRun())
	{
		if (!IsWallRunning)
		{
			IsWallRunning = true;
			CharRef->GetCharacterMovement()->Velocity = FVector(CharRef->GetVelocity().X, CharRef->GetVelocity().Y, FMath::Clamp(CharRef->GetVelocity().Z, -25.0, 35.0f));
			CharRef->GetCharacterMovement()->GravityScale = 0;
			
		}
	}
	else
	{
		IsWallRunning = false;
		CharRef->GetCharacterMovement()->GravityScale = 4.0;
	}
}

void UWallRunningComponent::WallRunningMovement()
{
	if(IsWallRunning)
	{
		CharRef->JumpCurrentCount = 0;
		FHitResult WallHitResult = bWallRightHit ? WallRightHitResult : WallLeftHitResult;
		
		WallNormal = WallHitResult.Normal;
		WallForward = FVector::CrossProduct(WallNormal, CharRef->GetActorUpVector());

		FVector ForwardVec = CharRef->GetActorForwardVector() - WallForward;
		FVector BackwardVec = CharRef->GetActorForwardVector() - WallForward * -1;
		
		if(ForwardVec.Length() > BackwardVec.Length())
		{
			WallForward = WallForward * -1;
		}

		FVector WallForceVector = WallForward * (WallRunAcceleration * 3.6f * 0.2778f * CharRef->GetCharacterMovement()->Mass * 100.0f);
		CharRef->GetCharacterMovement()->AddForce(WallForceVector);

		APlayerController* PC = Cast<APlayerController>(CharRef->GetController());
		bool IsHoldingLeft = PC && PC->IsInputKeyDown(EKeys::A);
		bool IsHoldingRight = PC && PC->IsInputKeyDown(EKeys::D);
		if(!IsHoldingLeft && !IsHoldingRight)
		{
			CharRef->GetCharacterMovement()->AddForce(WallNormal * -1.0f * 800000.0f);
		}

		// FVector AntiGravityForceVector = CharRef->GetActorUpVector() * (WallRunGravityCounterAcceleration * 3.6f * 0.2778f * CharRef->GetCharacterMovement()->Mass * 100.0f);
		// CharRef->GetCharacterMovement()->AddForce(AntiGravityForceVector);
		
		
		if(CharRef->GetVelocity().Length() > WallRunMaxSpeed)
		{
			// float ValueToClamp = WallRunKickOffDuration - WallRunKickOffCurrent;
			//
			// FVector2D InputRange(0.0f, WallRunSpeedLossDelay);
			// FVector2D OutputRange(0.0f, 1.0f);
			//
			// float MappedValue = FMath::GetMappedRangeValueClamped(InputRange, OutputRange, ValueToClamp);
			//
			// FVector StartVectorLerp = CharRef->GetVelocity();
			// FVector EndVectorLerp = CharRef->GetVelocity().GetSafeNormal(0.0001) * WallRunMaxSpeed;
			// FVector MaxVelocity = FMath::Lerp(StartVectorLerp, EndVectorLerp, MappedValue);
			//
			// UE_LOG(LogTemp, Log, TEXT("Vector: %s"), *MaxVelocity.ToString());
			//
			// CharRef->GetCharacterMovement()->Velocity = MaxVelocity;
			
			// Normalize the current velocity to get the direction
			FVector VelocityDirection = CharRef->GetVelocity().GetSafeNormal();

			// Scale it to the max allowed speed
			CharRef->GetCharacterMovement()->Velocity = VelocityDirection * WallRunMaxSpeed;
			
		}
		
	}
}

