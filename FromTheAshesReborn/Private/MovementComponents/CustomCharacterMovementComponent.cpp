#include "MovementComponents/CustomCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Characters/PlayableCharacter.h"
#include "RunTime/Engine/Classes/Kismet/KismetSystemLibrary.h"


void UCustomCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	bIsFalling = false;
	if (FSpringHeightCurve)
	{
		FSpringHeightCurve->GetTimeRange(SpringMinTime, SpringMaxTime);
	}

	if (FLeapHeightCurve)
	{
		FLeapHeightCurve->GetTimeRange(LeapMinTime, LeapMaxTime);
	}

}

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* TickFunction)
{
	Super::TickComponent(DeltaTime, TickType, TickFunction);

	ProcessCustomJump(DeltaTime);
	ProcessCustomFalling(DeltaTime);
}

bool UCustomCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	if (CharacterOwner && CharacterOwner->CanJump())
	{
		if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.0f)
		{
			APlayableCharacter* PlayerCharacter = Cast<APlayableCharacter>(GetOwner());
			
			if (!PlayerCharacter)
			{
				UE_LOG(LogTemp, Warning, TEXT("NO PC"));
				return false;
			}
			if (FLeapHeightCurve && PlayerCharacter->CanLeap)
			{
				SetMovementMode(EMovementMode::MOVE_Flying);

				IsJumping = true;
				IsLeaping = true;
				bIsFalling = false;

				CurrentTime = LeapMinTime;
				CurrentMaxTime = LeapMaxTime;

				FCurrentHeightCurve = FLeapHeightCurve;
				FCurrentDistanceCurve = FLeapDistanceCurve;

				PrevHeightCurveValue = FLeapHeightCurve->GetFloatValue(LeapMinTime);
				PrevDistanceCurveValue = FLeapDistanceCurve->GetFloatValue(LeapMinTime);

				return true;
			}
			
			else if (FSpringHeightCurve && JumpCount < 2)
			{
				SetMovementMode(EMovementMode::MOVE_Flying);

				IsJumping = true;
				bIsFalling = false;

				CurrentTime = SpringMinTime;
				CurrentMaxTime = SpringMaxTime;

				FCurrentHeightCurve = FSpringHeightCurve;
				FCurrentDistanceCurve = FSpringDistanceCurve;

				PrevHeightCurveValue = FSpringHeightCurve->GetFloatValue(SpringMinTime);
				PrevDistanceCurveValue = FSpringDistanceCurve->GetFloatValue(SpringMinTime);

				return true;
				
			}
			else
			{
				return Super::DoJump(bReplayingMoves);
			}
		}
	}
	return false;
}

bool UCustomCharacterMovementComponent::IsFalling() const
{
	if (FSpringHeightCurve)
	{
		return Super::IsFalling() || IsJumping || bIsFalling;
	}

	return Super::IsFalling();
}

void UCustomCharacterMovementComponent::SetCustomFallingMode()
{
	if (F_FallCurve)
	{
		bIsFalling = true;
		FallTime = 0.0f;
		PrevFallCurveValue = 0.0f;
		Velocity.Z = 0.0f;

		SetMovementMode(EMovementMode::MOVE_Flying);
		UE_LOG(LogTemp, Warning, TEXT("Movement mode set to flying"));

	}
	else
	{
		SetMovementMode(EMovementMode::MOVE_Falling);
		UE_LOG(LogTemp, Warning, TEXT("Movement mode set to MOVE_Falling"));
	}
}

void UCustomCharacterMovementComponent::ProcessCustomJump(float DeltaTime)
{
	if (IsJumping && FCurrentHeightCurve)
	{
		CurrentTime += DeltaTime;
		if (CurrentTime <= CurrentMaxTime)
		{
			float HeightCurveValue = FCurrentHeightCurve->GetFloatValue(CurrentTime);
			float HeightCurveDeltaValue = HeightCurveValue - PrevHeightCurveValue;
			PrevHeightCurveValue = HeightCurveValue;

			float DistanceCurveValue = FCurrentDistanceCurve->GetFloatValue(CurrentTime);
			float DistanceCurveDeltaValue = DistanceCurveValue - PrevDistanceCurveValue;
			PrevDistanceCurveValue = DistanceCurveValue;

			Velocity.Z = 0.0f;
			float Z_Velocity = HeightCurveDeltaValue / DeltaTime;

			FVector ActorLocation = GetActorLocation();

			//UE_LOG(LogTemp, Warning, TEXT("HeightCurveDeltaValue: %d"), HeightCurveDeltaValue);
			//UE_LOG(LogTemp, Warning, TEXT("DistanceCurveDeltaValue: %d"), DistanceCurveDeltaValue);

			FVector DestinationLocation = ActorLocation + (CharacterOwner->GetActorForwardVector() * DistanceCurveDeltaValue) + FVector(0.0f, 0.0f, HeightCurveDeltaValue);
			FVector HeightLocation = ActorLocation + FVector(0.0f, 0.0f, HeightCurveDeltaValue);

			if (Z_Velocity > 0.0f)
			{
				FCollisionQueryParams RoofCheckCollisionParams;
				RoofCheckCollisionParams.AddIgnoredActor(CharacterOwner);

				FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(),
					CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

				FHitResult RoofHitResult;
				bool IsBlockingHit = GetWorld()->SweepSingleByProfile(RoofHitResult, ActorLocation, DestinationLocation,
					CharacterOwner->GetActorRotation().Quaternion(), CharacterOwner->GetCapsuleComponent()->GetCollisionProfileName(),
					CapsuleShape, RoofCheckCollisionParams);

				if (IsBlockingHit)
				{
					IsJumping = false;
					CharacterOwner->ResetJumpState();
					Velocity.Z = 0.0f;
					DestinationLocation = ActorLocation;
					SetCustomFallingMode();

				}

			}
			
			if (Z_Velocity < 0.0f)
			{
				const FVector CapsuleLocation = UpdatedComponent->GetComponentLocation();
				FFindFloorResult FloorResult;
				bool IsFloor = CustomFindFloor(FloorResult, CapsuleLocation, DestinationLocation);
				const float FloorDistance = FloorResult.GetDistanceToFloor();

				
				if (IsFloor)
				{
					if (FloorDistance < FMath::Abs(HeightCurveDeltaValue))
					{
						DestinationLocation = CapsuleLocation - FVector(0.0f, 0.0f, HeightCurveDeltaValue);
					}
					UE_LOG(LogTemp, Warning, TEXT("Movement mode set to MOVE_Walking"));

					JumpCount = 0;
					SetMovementMode(EMovementMode::MOVE_Walking);
					IsJumping = false;
					CharacterOwner->ResetJumpState();
				}
				

			}
			
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			UKismetSystemLibrary::MoveComponentTo((USceneComponent*)CharacterOwner->GetCapsuleComponent(), DestinationLocation, CharacterOwner->GetActorRotation(), false, false, 0.0f, true, EMoveComponentAction::Move, LatentInfo);

		}

		else
		{
			
			//Reached end of curve
			const FVector CapsuleLocation = UpdatedComponent->GetComponentLocation();
			FFindFloorResult FloorResult;
			FindFloor(CapsuleLocation, FloorResult, false);
			if (FloorResult.IsWalkableFloor() && IsValidLandingSpot(CapsuleLocation, FloorResult.HitResult))
			{
				JumpCount = 0;
				SetMovementMode(MOVE_Walking);
				UE_LOG(LogTemp, Warning, TEXT("Movement mode set to MOVE_Walking"));
			}
			else
			{
				SetCustomFallingMode();
			}
			
			IsJumping = false;
			CharacterOwner->ResetJumpState();
		}
	}
}

void UCustomCharacterMovementComponent::ProcessCustomFalling(float DeltaTime)
{
	if (F_FallCurve)
	{
		if (bIsFalling)
		{
			FallTime += DeltaTime;
			float FallCurveValue = F_FallCurve->GetFloatValue(FallTime);
			float DeltaFallCurveValue = FallCurveValue - PrevFallCurveValue;
			PrevFallCurveValue = FallCurveValue;

			Velocity.Z = 0.0f;
			Velocity.Z = DeltaFallCurveValue / DeltaTime;

			FVector CapsuleLocation = UpdatedComponent->GetComponentLocation();

			FVector DestinationLocation = CapsuleLocation + FVector(0.0f, 0.0f, DeltaFallCurveValue);

			FFindFloorResult FloorResult;
			const bool IsFloor = CustomFindFloor(FloorResult, CapsuleLocation, DestinationLocation);
			
			if (IsFloor)
			{
				const float FloorDistance = FloorResult.GetDistanceToFloor();

				if (FloorDistance < FMath::Abs(DeltaFallCurveValue))
				{
					bIsFalling = false;
					DestinationLocation = CapsuleLocation - FVector(0.0f, 0.0f, FloorDistance);
					Velocity = FVector::ZeroVector;
					UE_LOG(LogTemp, Warning, TEXT("Movement mode set to MOVE_Walking"));

					SetMovementMode(EMovementMode::MOVE_Walking);
				}
			}
			
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			UKismetSystemLibrary::MoveComponentTo((USceneComponent*)CharacterOwner->GetCapsuleComponent(), DestinationLocation, CharacterOwner->GetActorRotation(), false, false, 0.0f, true, EMoveComponentAction::Move, LatentInfo);
		}
	}
	else if (MovementMode == EMovementMode::MOVE_Falling)
	{
		SetCustomFallingMode();
	}
}

bool UCustomCharacterMovementComponent::CustomFindFloor(FFindFloorResult& OutFloorResult, const FVector StartLocation, const FVector EndLocation)
{
	FCollisionQueryParams FloorCheckCollisionParams;
	FloorCheckCollisionParams.AddIgnoredActor(CharacterOwner);

	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(),
		CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	FHitResult FloorHitResult;
	bool IsBlockingHit = GetWorld()->SweepSingleByProfile(FloorHitResult, StartLocation, EndLocation,
		CharacterOwner->GetActorRotation().Quaternion(), CharacterOwner->GetCapsuleComponent()->GetCollisionProfileName(),
		CapsuleShape, FloorCheckCollisionParams);

	if (IsBlockingHit)
	{
		FindFloor(StartLocation, OutFloorResult, false, &FloorHitResult);
		return OutFloorResult.IsWalkableFloor() && IsValidLandingSpot(StartLocation, OutFloorResult.HitResult);
	}
	
	return false;
}
