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
#include "Kismet/KismetMathLibrary.h"
#include "Player/FTAPlayerCameraManger.h"
#include "Player/PlayerCharacter.h"

UTargetingSystemComponent::UTargetingSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	LockedOnWidgetClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/TargetSystem/UI/WBP_LockOn.WBP_LockOn_C"));
	TargetableActors = APawn::StaticClass();
	TargetableCollisionChannel = ECollisionChannel::ECC_Pawn;

	EnableSoftLockCameraOffset = true;
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

	if (!IsTargetLocked || !LockedOnTargetActor)
	{
		return;
	}

	if (!TargetIsTargetable(LockedOnTargetActor))
	{
		TargetLockOff();
		return;
	}
	// SetControlRotationOnTarget(LockedOnTargetActor);
	UpdateTargetingCameraAnchorAndRotation(PlayerCharacter, LockedOnTargetActor);
	DrawCameraAnchor();
	// ControlCameraOffset(DeltaTime);
	SetOwnerActorRotation();
	
	
	// Target Locked Off based on Distance
	if (GetDistanceFromCharacter(LockedOnTargetActor) > MinimumDistanceToEnable)
	{
		TargetLockOff();
	}

	if (ShouldBreakLineOfSight() && !IsBreakingLineOfSight)
	{
		if (BreakLineOfSightDelay <= 0)
		{
			TargetLockOff();
		}
		else
		{
			IsBreakingLineOfSight = true;
			GetWorld()->GetTimerManager().SetTimer(
				LineOfSightBreakTimerHandle,
				this,
				&UTargetingSystemComponent::BreakLineOfSight,
				BreakLineOfSightDelay
			);
		}
	}
}

void UTargetingSystemComponent::DrawCameraAnchor()
{
	DrawDebugSphere(
	GetWorld(),
	PlayerCharacter->TargetCameraAnchor->GetComponentLocation(),
	15.0f,              // radius
	12,                 // segments
	FColor::Red,        // color
	false,              // persistent lines
	-1.0f,              // lifetime
	0                   // depth priority
	);

}

AActor* UTargetingSystemComponent::TargetActor(bool& IsSuccess)
{
	ClosestTargetDistance = MinimumDistanceToEnable;
	
	if (IsTargetLocked)
	{
		TargetLockOff();
		IsSuccess = false;
		return nullptr;
	}
	
	const TArray<AActor*> Actors = GetAllActorsOfClass(TargetableActors);
	LockedOnTargetActor = FindNearestTargetToCenterViewport(Actors);
	
	TargetLockOn(LockedOnTargetActor);
	
	IsSuccess = true;
	FTAPlayerCameraManger->ViewPitchMax = 10;
	return LockedOnTargetActor;
}

void UTargetingSystemComponent::TargetActorWithAxisInput(const float AxisValue)
{
	// If we're not locked on, do nothing
	if (!IsTargetLocked)
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
	if (IsSwitchingTarget)
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
			IsSwitchingTarget ? 0.25f : 0.5f
		);

		IsSwitchingTarget = true;
	}
}

bool UTargetingSystemComponent::GetTargetLockedStatus()
{
	return IsTargetLocked;
}

AActor* UTargetingSystemComponent::GetLockedOnTargetActor() const
{
	return LockedOnTargetActor;
}

bool UTargetingSystemComponent::IsLocked() const
{
	return IsTargetLocked && LockedOnTargetActor;
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

float UTargetingSystemComponent::GetWorldDistanceFromCamera(APlayerController* PlayerController, const AActor* ActorToCheck)
{
	if(!PlayerController || !ActorToCheck)
	{
		UE_LOG(LogTemp, Error, TEXT("UTargetingSystemComponent::GetWorldDistanceFromCamera - Invalid Actors"));
		return 0.0f;
	}
	FVector CameraLocation;
	FRotator CameraRotation;

	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	return FVector::Distance(CameraLocation, ActorToCheck->GetActorLocation());
}

float UTargetingSystemComponent::CompareDistanceToScreenAndGetInterpSpeed(APlayerCharacter* PlayerOwner, const AActor* TargetActor, bool &InShouldUpdateControlRotation)
{
	float PlayerDistanceToScreen = GetWorldDistanceFromCamera(OwnerPlayerController, PlayerOwner);
	float TargetDistanceToScreen = GetWorldDistanceFromCamera(OwnerPlayerController, TargetActor);
	
	float DistanceToScreenDifference = TargetDistanceToScreen - PlayerDistanceToScreen;
	if(DistanceToScreenDifference < 0.0f)
	{
		InShouldUpdateControlRotation = false;
	}
	else
	{
		InShouldUpdateControlRotation = true;
	}
	
	float DistanceFactor = FMath::Clamp((DistanceToScreenDifference) / 100, 0.0f, 10.0f);
	return FMath::Lerp(0.0f, 1.0f, DistanceFactor);
}

void UTargetingSystemComponent::DisableMidPointControlRotation()
{
	PlayerCharacter->TargetCameraAnchor->SetRelativeLocation(PlayerCharacter->InitialSpringMeshLocation);
	PlayerCharacter->SpringArmComp->TargetArmLength = 400.0f;
}

void UTargetingSystemComponent::ControlCameraOffset(float DeltaTime)
{
	if (EnableSoftLockCameraOffset && OwnerPlayerController && IsTargetLocked)
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
	IsSwitchingTarget = false;
	DesireToSwitch = false;
}

bool UTargetingSystemComponent::ShouldSwitchTargetActor(const float AxisValue)
{
	if (EnableStickyTarget)
	{
		StartRotatingStack += (AxisValue != 0) ? AxisValue * AxisMultiplier : (StartRotatingStack > 0 ? -AxisMultiplier : AxisMultiplier);

		if (AxisValue == 0 && FMath::Abs(StartRotatingStack) <= AxisMultiplier)
		{
			StartRotatingStack = 0.0f;
		}

		if (FMath::Abs(StartRotatingStack) < StickyRotationThreshold)
		{
			DesireToSwitch = false;
			return false;
		}

		if (StartRotatingStack * AxisValue > 0)
		{
			StartRotatingStack = StartRotatingStack > 0 ? StickyRotationThreshold : -StickyRotationThreshold;
		}
		else if (StartRotatingStack * AxisValue < 0)
		{
			StartRotatingStack = StartRotatingStack * -1.0f;
		}

		DesireToSwitch = true;

		return true;
	}

	// Non Sticky feeling, check Axis value exceeds threshold
	return FMath::Abs(AxisValue) > StartRotatingThreshold;
}

void UTargetingSystemComponent::TargetLockOn(AActor* TargetToLockOn)
{
	if (!IsValid(TargetToLockOn))
	{
		UE_LOG(LogTemp, Error, TEXT("UTargetingSystemComponent::TargetLockOn - TargetToLockOn is invalid"));
		return;
	}

	SetupLocalPlayerController();

	IsTargetLocked = true;
	
	if (ShouldDrawLockedOnWidget)
	{
		CreateAndAttachTargetLockedOnWidgetComponent(TargetToLockOn);
	}

	if (ShouldControlRotation)
	{
		ControlRotation(true);
	}

	if (IgnoreLookInput)
	{
		if (IsValid(OwnerPlayerController))
		{
			OwnerPlayerController->SetIgnoreLookInput(false);
		}
	}

	if (OnTargetLockedOn.IsBound())
	{
		OnTargetLockedOn.Broadcast(TargetToLockOn);
	}
}

void UTargetingSystemComponent::TargetLockOff()
{
	SmoothedMidPoint = FVector::ZeroVector;
	IsTargetLocked = false;
	if (TargetLockedOnWidgetComponent)
	{
		TargetLockedOnWidgetComponent->DestroyComponent();
	}

	if (LockedOnTargetActor)
	{
		if (ShouldControlRotation)
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
	FTAPlayerCameraManger->ViewPitchMax = 30.0f;
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

	if (!IsValid(OwnerPlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("UTargetingSystemComponent::SetupLocalPlayerController() - OwnerPlayerController is invalid"));
		return;
	}

	FTAPlayerCameraManger = Cast<AFTAPlayerCameraManger>(OwnerPlayerController->PlayerCameraManager);

	if (!IsValid(FTAPlayerCameraManger))
	{
		UE_LOG(LogTemp, Error, TEXT("UTargetingSystemComponent::SetupLocalPlayerController() - FTAPlayerCameraManger is invalid"));
	}

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

	for (AActor* Actor : Actors)
	{
		TArray<AActor*> ActorsToIgnore;
		const bool bHit = LineTraceForActor(Actor, ActorsToIgnore);
		if (bHit && IsInViewport(Actor))
		{
			ActorsHit.Add(Actor);
		}
	}

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
	
	if (ShouldAdjustYawBasedOnDistanceToTarget || ShouldAdjustPitchBasedOnDistanceToTarget)
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
		if (IgnoreLookInput)
		{
			TargetRotation = FRotator(LookRotation.Pitch, Yaw, ControlRotation.Roll);
		}
		else
		{
			TargetRotation = FRotator(ControlRotation.Pitch, LookRotation.Yaw, ControlRotation.Roll);
		}
	}

	FRotator BlendedTargetRotation = TargetRotation;

	if (EnableSoftLockCameraOffset)
	{
		BlendedTargetRotation += CurrentCameraOffset;
	}

	return FMath::RInterpTo(ControlRotation, BlendedTargetRotation, GetWorld()->GetDeltaSeconds(), 9.0f);

}

void UTargetingSystemComponent::SetControlRotationOnTarget(AActor* TargetActor) const
{
	if (!IsValid(OwnerPlayerController))
	{
		UE_LOG(LogTemp, Warning, TEXT("UTargetingSystemComponent::SetControlRotationOnTarget - OwnerPlayerController is invalid"))
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

float UTargetingSystemComponent::CatchupToOffScreen(const FVector& PlayerLocation, float& InInterpSpeed)
{
	float InterpSpeed = InInterpSpeed;
	const float ScreenEdgeCatchupThreshold = 0.35f;

	FVector2D PlayerScreenPosition;
	if (OwnerPlayerController->ProjectWorldLocationToScreen(PlayerLocation, PlayerScreenPosition))
	{
		FVector2D ViewportSize;
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
		FVector2D ScreenCenter = ViewportSize * 0.5f;
		FVector2D Offset = PlayerScreenPosition - ScreenCenter;
		
		float OffsetMagnitude = (Offset / ViewportSize).Size();

		if (OffsetMagnitude > ScreenEdgeCatchupThreshold)
		{
			InterpSpeed = CatchupInterpSpeed;
		}
	}
	return InterpSpeed;
}

void UTargetingSystemComponent::UpdateTargetingCameraAnchorAndRotation(APlayerCharacter* PlayerOwner, const AActor* TargetActor)
{
	if (!PlayerOwner || !TargetActor || !OwnerPlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("UTargetingSystemComponent::UpdateTargetingCameraAnchorAndRotation - Invalid Access"))
		return;
	}
	
	const FVector PlayerLocation = PlayerOwner->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();
	FVector MidpointAnchorLocation = FMath::Lerp(PlayerLocation, TargetLocation, 0.5f);
	float Distance = FVector::Dist(PlayerLocation, TargetLocation);
	float DesiredRadius = Distance / 2.0f;

	DrawDebugSphere(
			GetWorld(),
			MidpointAnchorLocation,
			15.0f,          
			12,               
			FColor::Yellow,      
			false,              
			-1.0f,             
			0                  
			);

	if (SmoothedMidPoint.IsZero())
	{
		SmoothedMidPoint = MidpointAnchorLocation;
	}

	float OffScreenInterpSpeed = CatchupToOffScreen(PlayerLocation, CatchupInterpSpeed);

	SmoothedMidPoint = FMath::VInterpTo(SmoothedMidPoint, MidpointAnchorLocation, GetWorld()->GetDeltaSeconds(), OffScreenInterpSpeed);

	if (IsValid(PlayerOwner->TargetCameraAnchor))
	{
		PlayerOwner->TargetCameraAnchor->SetWorldLocation(SmoothedMidPoint);

		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(SmoothedMidPoint, TargetLocation);
		const FRotator NewRotation = FMath::RInterpTo(PlayerOwner->TargetCameraAnchor->GetComponentRotation(), LookAtRotation, GetWorld()->GetDeltaSeconds(), 8.0f);
		PlayerOwner->TargetCameraAnchor->SetWorldRotation(NewRotation);
	}

	if (IsValid(PlayerOwner->SpringArmComp))
	{
		const float TargetArmLength = DesiredRadius + 300.0f;
		PlayerOwner->SpringArmComp->TargetArmLength = FMath::FInterpTo(PlayerOwner->SpringArmComp->TargetArmLength, TargetArmLength, GetWorld()->GetDeltaSeconds(), 6.0f);
	}

	float ControlRotationInterpSpeed = CompareDistanceToScreenAndGetInterpSpeed(PlayerOwner, TargetActor, ShouldUpdateControllerRotation);
	if (ShouldUpdateControllerRotation)
	{
		FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, TargetLocation);
		FRotator FinalRot = FMath::RInterpTo(OwnerPlayerController->GetControlRotation(), LookRot, GetWorld()->GetDeltaSeconds(), ControlRotationInterpSpeed);
		OwnerPlayerController->SetControlRotation(FinalRot);
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
	IsBreakingLineOfSight = false;
	if (ShouldBreakLineOfSight())
	{
		TargetLockOff();
	}
}

void UTargetingSystemComponent::ControlRotation(const bool InShouldControlRotation) const
{
	if (!IsValid(OwnerPawn))
	{
		return;
	}

	OwnerPawn->bUseControllerRotationYaw = InShouldControlRotation;

	UCharacterMovementComponent* CharacterMovementComponent = OwnerPawn->FindComponentByClass<UCharacterMovementComponent>();
	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->bOrientRotationToMovement = !InShouldControlRotation;
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
