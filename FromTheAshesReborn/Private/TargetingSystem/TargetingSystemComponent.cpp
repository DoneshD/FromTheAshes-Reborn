#include "TargetingSystem/TargetingSystemComponent.h"
#include "EngineUtils.h"
#include "TargetingSystem/TargetingSystemTargetableInterface.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/PlayerCharacter.h"

UTargetingSystemComponent::UTargetingSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	LockedOnWidgetClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/TargetSystem/UI/WBP_LockOn.WBP_LockOn_C"));
	TargetableActors = APawn::StaticClass();
	TargetableCollisionChannel = ECollisionChannel::ECC_Pawn;

	bEnableSoftLockCameraOffset = true;
	MaxSoftYawOffset = 25.0f;
	MaxSoftPitchOffset = 10.0f;
	SoftLockDecayRate = 3.0f;
	CurrentCameraOffset = FRotator::ZeroRotator;

}

void UTargetingSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] TargetSystemComponent: Cannot get Owner reference ..."), *GetName());
		return;
	}

	OwnerPawn = Cast<APawn>(OwnerActor);
	if (!ensure(OwnerPawn))
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] TargetSystemComponent: Component is meant to be added to Pawn only ..."), *GetName());
		return;
	}

	PlayerCharacter = Cast<APlayerCharacter>(OwnerPawn);
	if (!ensure(PlayerCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] TargetSystemComponent: Component is meant to be added to PlayerCharacter only ..."), *GetName());
		return;
	}

	SetupLocalPlayerController();
}

void UTargetingSystemComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bTargetLocked || !LockedOnTargetActor)
	{
		return;
	}

	if (!TargetIsTargetable(LockedOnTargetActor))
	{
		TargetLockOff();
		return;
	}
	UpdateMidPointControlRotation(PlayerCharacter, LockedOnTargetActor);
	SetControlRotationOnTarget(LockedOnTargetActor);
	ControlCameraOffset(DeltaTime);
	SetOwnerActorRotation();
	
	
	// Target Locked Off based on Distance
	if (GetDistanceFromCharacter(LockedOnTargetActor) > MinimumDistanceToEnable)
	{
		TargetLockOff();
	}

	if (ShouldBreakLineOfSight() && !bIsBreakingLineOfSight)
	{
		if (BreakLineOfSightDelay <= 0)
		{
			TargetLockOff();
		}
		else
		{
			bIsBreakingLineOfSight = true;
			GetWorld()->GetTimerManager().SetTimer(
				LineOfSightBreakTimerHandle,
				this,
				&UTargetingSystemComponent::BreakLineOfSight,
				BreakLineOfSightDelay
			);
		}
	}
}

AActor* UTargetingSystemComponent::TargetActor(bool& bSuccess)
{
	FVector2D CenterVec = FindCenterOfViewPort();
	
	ClosestTargetDistance = MinimumDistanceToEnable;
	if (bTargetLocked)
	{
		TargetLockOff();
		bSuccess = false;
		return nullptr;
	}
	else
	{
		const TArray<AActor*> Actors = GetAllActorsOfClass(TargetableActors);
		LockedOnTargetActor = FindNearestTargetToCenterViewport(Actors);
		
		TargetLockOn(LockedOnTargetActor);
		bSuccess = true;
		return LockedOnTargetActor;
		
	}
}

void UTargetingSystemComponent::TargetActorWithAxisInput(const float AxisValue)
{
	// If we're not locked on, do nothing
	if (!bTargetLocked)
	{
		return;
	}

	if (!LockedOnTargetActor)
	{
		return;
	}

	// If we're not allowed to switch target, do nothing
	if (!ShouldSwitchTargetActor(AxisValue))
	{
		return;
	}

	// If we're switching target, do nothing for a set amount of time
	if (bIsSwitchingTarget)
	{
		return;
	}

	// Lock off target
	AActor* CurrentTarget = LockedOnTargetActor;

	// Depending on Axis Value negative / positive, set Direction to Look for (negative: left, positive: right)
	const float RangeMin = AxisValue < 0 ? 0 : 180;
	const float RangeMax = AxisValue < 0 ? 180 : 360;

	// Reset Closest Target Distance to Minimum Distance to Enable
	ClosestTargetDistance = MinimumDistanceToEnable;

	// Get All Actors of Class
	TArray<AActor*> Actors = GetAllActorsOfClass(TargetableActors);

	// For each of these actors, check line trace and ignore Current Target and build the list of actors to look from
	TArray<AActor*> ActorsToLook;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(CurrentTarget);
	for (AActor* Actor : Actors)
	{
		const bool bHit = LineTraceForActor(Actor, ActorsToIgnore);
		if (bHit && IsInViewport(Actor))
		{
			ActorsToLook.Add(Actor);
		}
	}

	// Find Targets in Range (left or right, based on Character and CurrentTarget)
	TArray<AActor*> TargetsInRange = FindTargetsInRange(ActorsToLook, RangeMin, RangeMax);

	// For each of these targets in range, get the closest one to current target
	AActor* ActorToTarget = nullptr;
	for (AActor* Actor : TargetsInRange)
	{
		// and filter out any character too distant from minimum distance to enable
		const float Distance = GetDistanceFromCharacter(Actor);
		if (Distance < MinimumDistanceToEnable)
		{
			const float RelativeActorsDistance = CurrentTarget->GetDistanceTo(Actor);
			if (RelativeActorsDistance < ClosestTargetDistance)
			{
				ClosestTargetDistance = RelativeActorsDistance;
				ActorToTarget = Actor;
			}
		}
	}

	if (ActorToTarget)
	{
		if (SwitchingTargetTimerHandle.IsValid())
		{
			SwitchingTargetTimerHandle.Invalidate();
		}

		TargetLockOff();
		LockedOnTargetActor = ActorToTarget;
		TargetLockOn(ActorToTarget);

		GetWorld()->GetTimerManager().SetTimer(
			SwitchingTargetTimerHandle,
			this,
			&UTargetingSystemComponent::ResetIsSwitchingTarget,
			// Less sticky if still switching
			bIsSwitchingTarget ? 0.25f : 0.5f
		);

		bIsSwitchingTarget = true;
	}
}

bool UTargetingSystemComponent::GetTargetLockedStatus()
{
	return bTargetLocked;
}

AActor* UTargetingSystemComponent::GetLockedOnTargetActor() const
{
	return LockedOnTargetActor;
}

bool UTargetingSystemComponent::IsLocked() const
{
	return bTargetLocked && LockedOnTargetActor;
}

TArray<AActor*> UTargetingSystemComponent::FindTargetsInRange(TArray<AActor*> ActorsToLook, const float RangeMin, const float RangeMax) const
{
	TArray<AActor*> ActorsInRange;

	for (AActor* Actor : ActorsToLook)
	{
		const float Angle = GetAngleUsingCameraRotation(Actor);
		if (Angle > RangeMin && Angle < RangeMax)
		{
			ActorsInRange.Add(Actor);
		}
	}

	return ActorsInRange;
}

float UTargetingSystemComponent::GetAngleUsingCameraRotation(const AActor* ActorToLook) const
{
	UCameraComponent* CameraComponent = OwnerActor->FindComponentByClass<UCameraComponent>();
	if (!CameraComponent)
	{
		// Fallback to CharacterRotation if no CameraComponent can be found
		return GetAngleUsingCharacterRotation(ActorToLook);
	}

	const FRotator CameraWorldRotation = CameraComponent->GetComponentRotation();
	const FRotator LookAtRotation = FindLookAtRotation(CameraComponent->GetComponentLocation(), ActorToLook->GetActorLocation());

	float YawAngle = CameraWorldRotation.Yaw - LookAtRotation.Yaw;
	if (YawAngle < 0)
	{
		YawAngle = YawAngle + 360;
	}

	return YawAngle;
}

float UTargetingSystemComponent::GetAngleUsingCharacterRotation(const AActor* ActorToLook) const
{
	const FRotator CharacterRotation = OwnerActor->GetActorRotation();
	const FRotator LookAtRotation = FindLookAtRotation(OwnerActor->GetActorLocation(), ActorToLook->GetActorLocation());

	float YawAngle = CharacterRotation.Yaw - LookAtRotation.Yaw;
	if (YawAngle < 0)
	{
		YawAngle = YawAngle + 360;
	}

	return YawAngle;
}

FRotator UTargetingSystemComponent::FindLookAtRotation(const FVector Start, const FVector Target)
{
	return FRotationMatrix::MakeFromX(Target - Start).Rotator();
}

FVector UTargetingSystemComponent::CalculateMidpoint(FVector PlayerLocation, FVector TargetLocation)
{
	FVector Midpoint = (PlayerLocation + TargetLocation) / 2.0f;

	return Midpoint;
}

float UTargetingSystemComponent::CalculateDistance(FVector PlayerLocation, FVector TargetLocation)
{
	float CalculateX = FMath::Square(abs((PlayerLocation.X - TargetLocation.X)));
	float CalculateY = FMath::Square(abs((PlayerLocation.Y - TargetLocation.Y)));
	float CalculateZ = FMath::Square(abs((PlayerLocation.Z - TargetLocation.Z)));

	return FMath::Sqrt(CalculateX + CalculateY + CalculateZ);

}

void UTargetingSystemComponent::UpdateMidPointControlRotation(APlayerCharacter* PlayerOwner, const AActor* TargetActor)
{
	if (!IsValid(PlayerOwner) || !IsValid(TargetActor)) return;

	const FVector PlayerLoc = PlayerOwner->GetActorLocation();
	const FVector TargetLoc = TargetActor->GetActorLocation();

	FVector DesiredMidpoint = FMath::Lerp(PlayerLoc, TargetLoc, 0.5f);
	float Distance = FVector::Dist(PlayerLoc, TargetLoc);
	float DesiredRadius = Distance / 2.0f;

	if (SmoothedMidPoint.IsZero())
	{
		SmoothedMidPoint = DesiredMidpoint;
	}
	
	float InterpSpeed = 8.0f; 
	const float CatchupInterpSpeed = 18.0f;
	const float ScreenEdgeCatchupThreshold = 0.35f; 

	if (IsValid(OwnerPlayerController))
	{
		FVector2D PlayerScreenPosition;
		if (OwnerPlayerController->ProjectWorldLocationToScreen(PlayerOwner->GetActorLocation(), PlayerScreenPosition))
		{
			FVector2D ViewportSize;
			GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

			FVector2D ScreenCenter = ViewportSize * 0.5f;
			FVector2D DistanceFromCenter = PlayerScreenPosition - ScreenCenter;

			FVector2D NormalizedOffset = DistanceFromCenter / ViewportSize;

			float OffsetMagnitude = FVector2D(FMath::Abs(NormalizedOffset.X), FMath::Abs(NormalizedOffset.Y)).Size();

			if (OffsetMagnitude > ScreenEdgeCatchupThreshold)
			{
				InterpSpeed = CatchupInterpSpeed;
			}
		}
	}

	SmoothedMidPoint = FMath::VInterpTo(SmoothedMidPoint, DesiredMidpoint, GetWorld()->GetDeltaSeconds(), InterpSpeed);

	if (IsValid(PlayerOwner->TargetCameraAnchor))
	{
		PlayerOwner->TargetCameraAnchor->SetWorldLocation(SmoothedMidPoint);
	}

	if (IsValid(PlayerOwner->SpringArmComp))
	{
		float CurrentArmLength = PlayerOwner->SpringArmComp->TargetArmLength;
		float TargetArmLength = DesiredRadius + 300.0f;
		float NewArmLength = FMath::FInterpTo(CurrentArmLength, TargetArmLength, GetWorld()->GetDeltaSeconds(), 6.0f);
		PlayerOwner->SpringArmComp->TargetArmLength = NewArmLength;
	}
}

void UTargetingSystemComponent::DisableMidPointControlRotation()
{
	PlayerCharacter->TargetCameraAnchor->SetRelativeLocation(PlayerCharacter->InitialSpringMeshLocation);
	PlayerCharacter->SpringArmComp->TargetArmLength = 400.0f;
}

void UTargetingSystemComponent::ControlCameraOffset(float DeltaTime)
{
	if (bEnableSoftLockCameraOffset && OwnerPlayerController && bTargetLocked)
	{
		float YawInput = 0.0f;
		float PitchInput = 0.0f;

		OwnerPlayerController->GetInputMouseDelta(YawInput, PitchInput);

		const float InputScale = CameraInputScale;
		YawInput *= InputScale;
		PitchInput *= InputScale;

		CurrentCameraOffset.Yaw += YawInput;
		CurrentCameraOffset.Pitch += PitchInput;

		CurrentCameraOffset.Yaw = FMath::Clamp(CurrentCameraOffset.Yaw, -MaxSoftYawOffset, MaxSoftYawOffset);
		CurrentCameraOffset.Pitch = FMath::Clamp(CurrentCameraOffset.Pitch, -MaxSoftPitchOffset, MaxSoftPitchOffset);

		const float DecayRate = SoftLockDecayRate * DeltaTime;
		CurrentCameraOffset = FMath::RInterpTo(CurrentCameraOffset, FRotator::ZeroRotator, DeltaTime, SoftLockDecayRate);
	}
}

void UTargetingSystemComponent::ResetIsSwitchingTarget()
{
	bIsSwitchingTarget = false;
	bDesireToSwitch = false;
}

bool UTargetingSystemComponent::ShouldSwitchTargetActor(const float AxisValue)
{
	// Sticky feeling computation
	if (bEnableStickyTarget)
	{
		StartRotatingStack += (AxisValue != 0) ? AxisValue * AxisMultiplier : (StartRotatingStack > 0 ? -AxisMultiplier : AxisMultiplier);

		if (AxisValue == 0 && FMath::Abs(StartRotatingStack) <= AxisMultiplier)
		{
			StartRotatingStack = 0.0f;
		}

		// If Axis value does not exceeds configured threshold, do nothing
		if (FMath::Abs(StartRotatingStack) < StickyRotationThreshold)
		{
			bDesireToSwitch = false;
			return false;
		}

		//Sticky when switching target.
		if (StartRotatingStack * AxisValue > 0)
		{
			StartRotatingStack = StartRotatingStack > 0 ? StickyRotationThreshold : -StickyRotationThreshold;
		}
		else if (StartRotatingStack * AxisValue < 0)
		{
			StartRotatingStack = StartRotatingStack * -1.0f;
		}

		bDesireToSwitch = true;

		return true;
	}

	// Non Sticky feeling, check Axis value exceeds threshold
	return FMath::Abs(AxisValue) > StartRotatingThreshold;
}

void UTargetingSystemComponent::TargetLockOn(AActor* TargetToLockOn)
{
	if (!IsValid(TargetToLockOn))
	{
		return;
	}

	// Recast PlayerController in case it wasn't already setup on Begin Play (local split screen)
	SetupLocalPlayerController();

	bTargetLocked = true;
	if (bShouldDrawLockedOnWidget)
	{
		CreateAndAttachTargetLockedOnWidgetComponent(TargetToLockOn);
	}

	if (bShouldControlRotation)
	{
		ControlRotation(true);
	}

	if (bAdjustPitchBasedOnDistanceToTarget || bIgnoreLookInput)
	{
		if (IsValid(OwnerPlayerController))
		{
			OwnerPlayerController->SetIgnoreLookInput(true);
		}
	}

	if (OnTargetLockedOn.IsBound())
	{
		OnTargetLockedOn.Broadcast(TargetToLockOn);
	}
}

void UTargetingSystemComponent::TargetLockOff()
{
	// Recast PlayerController in case it wasn't already setup on Begin Play (local split screen)
	SetupLocalPlayerController();
	SmoothedMidPoint = FVector::ZeroVector;
	bTargetLocked = false;
	if (TargetLockedOnWidgetComponent)
	{
		TargetLockedOnWidgetComponent->DestroyComponent();
	}

	if (LockedOnTargetActor)
	{
		if (bShouldControlRotation)
		{
			ControlRotation(false);
		}

		if (IsValid(OwnerPlayerController))
		{
			OwnerPlayerController->ResetIgnoreLookInput();
		}

		if (OnTargetLockedOff.IsBound())
		{
			OnTargetLockedOff.Broadcast(LockedOnTargetActor);
		}
	}

	DisableMidPointControlRotation();
	LockedOnTargetActor = nullptr;
}

void UTargetingSystemComponent::CreateAndAttachTargetLockedOnWidgetComponent(AActor* TargetActor)
{
	if (!LockedOnWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetSystemComponent: Cannot get LockedOnWidgetClass, please ensure it is a valid reference in the Component Properties."));
		return;
	}

	TargetLockedOnWidgetComponent = NewObject<UWidgetComponent>(TargetActor, MakeUniqueObjectName(TargetActor, UWidgetComponent::StaticClass(), FName("TargetLockOn")));
	TargetLockedOnWidgetComponent->SetWidgetClass(LockedOnWidgetClass);

	UMeshComponent* MeshComponent = TargetActor->FindComponentByClass<UMeshComponent>();
	USceneComponent* ParentComponent = MeshComponent && LockedOnWidgetParentSocket != NAME_None ? MeshComponent : TargetActor->GetRootComponent();

	if (IsValid(OwnerPlayerController))
	{
		TargetLockedOnWidgetComponent->SetOwnerPlayer(OwnerPlayerController->GetLocalPlayer());
	}

	TargetLockedOnWidgetComponent->ComponentTags.Add(FName("TargetSystem.LockOnWidget"));
	TargetLockedOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	TargetLockedOnWidgetComponent->SetupAttachment(ParentComponent, LockedOnWidgetParentSocket);
	TargetLockedOnWidgetComponent->SetRelativeLocation(LockedOnWidgetRelativeLocation);
	TargetLockedOnWidgetComponent->SetDrawSize(FVector2D(LockedOnWidgetDrawSize, LockedOnWidgetDrawSize));
	TargetLockedOnWidgetComponent->SetVisibility(true);
	TargetLockedOnWidgetComponent->RegisterComponent();
}

TArray<AActor*> UTargetingSystemComponent::GetAllActorsOfClass(const TSubclassOf<AActor> ActorClass) const
{
	TArray<AActor*> Actors;
	for (TActorIterator<AActor> ActorIterator(GetWorld(), ActorClass); ActorIterator; ++ActorIterator)
	{
		AActor* Actor = *ActorIterator;
		const bool bIsTargetable = TargetIsTargetable(Actor);
		if (bIsTargetable)
		{
			Actors.Add(Actor);
		}
	}

	return Actors;
}

bool UTargetingSystemComponent::TargetIsTargetable(const AActor* Actor)
{
	const bool bIsImplemented = Actor->GetClass()->ImplementsInterface(UTargetingSystemTargetableInterface::StaticClass());
	if (bIsImplemented)
	{
		return ITargetingSystemTargetableInterface::Execute_IsTargetable(Actor);
	}

	return true;
}

void UTargetingSystemComponent::SetupLocalPlayerController()
{
	if (!IsValid(OwnerPawn))
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] TargetSystemComponent: Component is meant to be added to Pawn only ..."), *GetName());
		return;
	}

	OwnerPlayerController = Cast<APlayerController>(OwnerPawn->GetController());
}

AActor* UTargetingSystemComponent::FindNearestTargetToActor(TArray<AActor*> Actors) const
{
	TArray<AActor*> ActorsHit;

	// Find all actors we can line trace to
	for (AActor* Actor : Actors)
	{
		TArray<AActor*> ActorsToIgnore;
		const bool bHit = LineTraceForActor(Actor, ActorsToIgnore);
		if (bHit && IsInViewport(Actor))
		{
			ActorsHit.Add(Actor);
		}
	}

	// From the hit actors, check distance and return the nearest
	if (ActorsHit.Num() == 0)
	{
		return nullptr;
	}
	
	float ClosestDistance = ClosestTargetDistance;
	AActor* Target = nullptr;
	
	for (AActor* Actor : ActorsHit)
	{
		const float Distance = GetDistanceFromCharacter(Actor);
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			Target = Actor;
		}
	}

	return Target;
}

AActor* UTargetingSystemComponent::FindNearestTargetToCenterViewport(TArray<AActor*> Actors) const
{
	TArray<AActor*> ActorsHit;

	// Find all actors we can line trace to
	for (AActor* Actor : Actors)
	{
		TArray<AActor*> ActorsToIgnore;
		const bool bHit = LineTraceForActor(Actor, ActorsToIgnore);
		if (bHit && IsInViewport(Actor))
		{
			ActorsHit.Add(Actor);
		}
	}

	// From the hit actors, check distance and return the nearest
	if (ActorsHit.Num() == 0)
	{
		return nullptr;
	}
	
	float ClosestDistance = ClosestTargetDistance;
	AActor* Target = nullptr;
	for (AActor* Actor : ActorsHit)
	{
		const float DistanceFromCenter = FindDistanceFromCenterOfViewport(Actor);
		if (DistanceFromCenter < ClosestDistance)
		{
			ClosestDistance = DistanceFromCenter;
			Target = Actor;
		}
	}

	return Target;
}


bool UTargetingSystemComponent::LineTraceForActor(const AActor* OtherActor, const TArray<AActor*>& ActorsToIgnore) const
{
	FHitResult HitResult;
	const bool bHit = LineTrace(HitResult, OtherActor, ActorsToIgnore);
	if (bHit)
	{
		const AActor* HitActor = HitResult.GetActor();
		if (HitActor == OtherActor)
		{
			return true;
		}
	}

	return false;
}

bool UTargetingSystemComponent::LineTrace(FHitResult& OutHitResult, const AActor* OtherActor, const TArray<AActor*>& ActorsToIgnore) const
{
	if (!IsValid(OwnerActor))
	{
		UE_LOG(LogTemp, Error, TEXT("UTargetSystemComponent::LineTrace - Called with invalid OwnerActor: %s"), *GetNameSafe(OwnerActor))
		return false;
	}
	
	if (!IsValid(OtherActor))
	{
		UE_LOG(LogTemp, Error, TEXT("UTargetSystemComponent::LineTrace - Called with invalid OtherActor: %s"), *GetNameSafe(OtherActor))
		return false;
	}
	
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Reserve(ActorsToIgnore.Num() + 1);
	IgnoredActors.Add(OwnerActor);
	IgnoredActors.Append(ActorsToIgnore);
	
	FCollisionQueryParams Params = FCollisionQueryParams(FName("LineTraceSingle"));
	Params.AddIgnoredActors(IgnoredActors);
	
	if (const UWorld* World = GetWorld(); IsValid(World))
	{
		return World->LineTraceSingleByChannel(
			OutHitResult,
			OwnerActor->GetActorLocation(),
			OtherActor->GetActorLocation(),
			TargetableCollisionChannel,
			Params
		);
	}

	UE_LOG(LogTemp, Error, TEXT("UTargetSystemComponent::LineTrace - Called with invalid World: %s"), *GetNameSafe(GetWorld()))
	return false;
}

float UTargetingSystemComponent::CalculateControlRotationOffset(float Distance, float MaxOffset) const
{
	if (Distance > MaxDistance)
	{
		return 0.0f;
	}

	float DistanceFactor = 1.0f - FMath::Clamp((Distance - MinDistance) / (MaxDistance - MinDistance), 0.0f, 1.0f);
	return FMath::Lerp(0.0f, MaxOffset, DistanceFactor);
}

FRotator UTargetingSystemComponent::GetControlRotationOnTarget(const AActor* OtherActor) const
{
	if (!IsValid(OwnerPlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("UTargetSystemComponent::GetControlRotationOnTarget - OwnerPlayerController is not valid ..."))
		return FRotator::ZeroRotator;
	}

	const FRotator ControlRotation = OwnerPlayerController->GetControlRotation();

	const FVector CharacterLocation = OwnerActor->GetActorLocation();
	const FVector OtherActorLocation = OtherActor->GetActorLocation();

	// Find look at rotation
	const FRotator LookRotation = FindLookAtRotation(CharacterLocation,OtherActorLocation);
	float Yaw = LookRotation.Yaw;
	float Pitch = LookRotation.Pitch;
	
	FRotator TargetRotation;
	
	if (bAdjustYawBasedOnDistanceToTarget || bAdjustPitchBasedOnDistanceToTarget)
	{
		const float DistanceToTarget = GetDistanceFromCharacter(OtherActor);
		
		float DesiredPitch = CalculateControlRotationOffset(DistanceToTarget, MaxPitchOffset);
		float DesiredYaw = CalculateControlRotationOffset(DistanceToTarget, MaxYawOffset);
		
		Pitch = Pitch + DesiredPitch;
		Yaw = Yaw + DesiredYaw;
		
		TargetRotation = FRotator(Pitch, Yaw, ControlRotation.Roll);
	}
	else
	{
		if (bIgnoreLookInput)
		{
			TargetRotation = FRotator(LookRotation.Pitch, Yaw, ControlRotation.Roll);
		}
		else
		{
			TargetRotation = FRotator(ControlRotation.Pitch, LookRotation.Yaw, ControlRotation.Roll);
		}
	}

	FRotator BlendedTargetRotation = TargetRotation;

	if (bEnableSoftLockCameraOffset)
	{
		BlendedTargetRotation += CurrentCameraOffset;
	}

	return FMath::RInterpTo(ControlRotation, BlendedTargetRotation, GetWorld()->GetDeltaSeconds(), 9.0f);

}

void UTargetingSystemComponent::SetControlRotationOnTarget(AActor* TargetActor) const
{
	if (!IsValid(OwnerPlayerController))
	{
		return;
	}

	const FRotator ControlRotation = GetControlRotationOnTarget(TargetActor);
	if (OnTargetSetRotation.IsBound())
	{
		OnTargetSetRotation.Broadcast(TargetActor, ControlRotation);
	}
	else
	{
		OwnerPlayerController->SetControlRotation(ControlRotation);
	}
}

float UTargetingSystemComponent::GetDistanceFromCharacter(const AActor* OtherActor) const
{
	return OwnerActor->GetDistanceTo(OtherActor);
}

void UTargetingSystemComponent::SetOwnerActorRotation()
{
	FVector TargetLocation = LockedOnTargetActor->GetActorLocation();
	FRotator TargetRotation = FindLookAtRotation(OwnerActor->GetActorLocation(), TargetLocation);
	FRotator InterpRot = FMath::RInterpTo(OwnerActor->GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 10.0f);

	OwnerActor->SetActorRotation(InterpRot);
}

bool UTargetingSystemComponent::ShouldBreakLineOfSight() const
{
	if (!LockedOnTargetActor)
	{
		return true;
	}

	TArray<AActor*> ActorsToIgnore = GetAllActorsOfClass(TargetableActors);
	ActorsToIgnore.Remove(LockedOnTargetActor);

	FHitResult HitResult;
	const bool bHit = LineTrace(HitResult, LockedOnTargetActor, ActorsToIgnore);
	if (bHit && HitResult.GetActor() != LockedOnTargetActor)
	{
		return true;
	}

	return false;
}

void UTargetingSystemComponent::BreakLineOfSight()
{
	bIsBreakingLineOfSight = false;
	if (ShouldBreakLineOfSight())
	{
		TargetLockOff();
	}
}

void UTargetingSystemComponent::ControlRotation(const bool ShouldControlRotation) const
{
	if (!IsValid(OwnerPawn))
	{
		return;
	}

	OwnerPawn->bUseControllerRotationYaw = ShouldControlRotation;

	UCharacterMovementComponent* CharacterMovementComponent = OwnerPawn->FindComponentByClass<UCharacterMovementComponent>();
	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->bOrientRotationToMovement = !ShouldControlRotation;
	}
}

bool UTargetingSystemComponent::IsInViewport(const AActor* TargetActor) const
{
	if (!IsValid(OwnerPlayerController))
	{
		return true;
	}

	FVector2D ScreenLocation;
	OwnerPlayerController->ProjectWorldLocationToScreen(TargetActor->GetActorLocation(), ScreenLocation);

	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

	return ScreenLocation.X > 0 && ScreenLocation.Y > 0 && ScreenLocation.X < ViewportSize.X && ScreenLocation.Y < ViewportSize.Y;
}

float UTargetingSystemComponent::FindDistanceFromCenterOfViewport(const AActor* TargetActor) const
{
	FVector ActorLocation = TargetActor->GetActorLocation();
	FVector2D ScreenPosition;
	
	OwnerPlayerController->ProjectWorldLocationToScreen(ActorLocation, ScreenPosition);
	
	FVector2D CenterOfScreen = FindCenterOfViewPort();
	
	return FVector2D::Distance(CenterOfScreen, ScreenPosition);
	
}

FVector2D UTargetingSystemComponent::FindCenterOfViewPort() const
{
	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

	//Need to make offset variable from spring component
	FVector2D CenterViewportSize = FVector2D((ViewportSize.X * 0.5f) - 40.0f, ViewportSize.Y * 0.5f);
	
	return CenterViewportSize;
}
