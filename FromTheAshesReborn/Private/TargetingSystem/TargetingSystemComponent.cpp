#include "TargetingSystem/TargetingSystemComponent.h"

#include "CameraSystemComponent.h"
#include "CameraSystemParams.h"
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
#include "HelperFunctionLibraries/ViewportUtilityFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/FTAPlayerCameraManger.h"
#include "Player/PlayerCharacter.h"

UTargetingSystemComponent::UTargetingSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	LockedOnWidgetClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/TargetSystem/UI/WBP_LockOn.WBP_LockOn_C"));
	TargetableActors = APawn::StaticClass();
	TargetableCollisionChannel = ECollisionChannel::ECC_Pawn;

	EnableInputBasedOffset = true;
	InputBasedMaxYawOffset = 25.0f;
	InputBasedMaxPitchOffset = 10.0f;
	InputOffsetDecayRate = 3.0f;
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
		DisableMidPointControlRotation();
	}
	else
	{
		ControlCameraOffset(DeltaTime);
		UpdateTargetingCameraAnchorAndRotation(PlayerCharacter, LockedOnTargetActor);
		// // DrawCameraAnchor();
		SetOwnerActorRotation();
	}

	if (!IsTargetLocked || !LockedOnTargetActor)
	{
		return;
	}

	if (!TargetIsTargetable(LockedOnTargetActor))
	{
		TargetLockOff();
		return;
	}
	
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
	PlayerCharacter->CameraAnchorComponent->GetComponentLocation(),
	15.0f,           
	12,                 
	FColor::Red,        
	false,              
	-1.0f,              
	0                   
	);

}

void UTargetingSystemComponent::TargetLockOn(AActor* TargetToLockOn)
{
	if (!IsValid(TargetToLockOn))
	{
		UE_LOG(LogTemp, Error, TEXT("UTargetingSystemComponent::TargetLockOn - TargetToLockOn is invalid"));
		return;
	}

	SetupLocalPlayerController();
	
	bIsLockingOn = true;
	IsTargetLocked = true;
	
	if (ShouldDrawLockedOnWidget)
	{
		CreateAndAttachTargetLockedOnWidgetComponent(TargetToLockOn);
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

bool UTargetingSystemComponent::TargetIsTargetable(const AActor* Actor)
{
	const bool bIsImplemented = Actor->GetClass()->ImplementsInterface(UTargetingSystemTargetableInterface::StaticClass());
	if (bIsImplemented)
	{
		return ITargetingSystemTargetableInterface::Execute_IsTargetable(Actor);
	}

	return true;
}

void UTargetingSystemComponent::UpdateTargetingCameraAnchorAndRotation(APlayerCharacter* PlayerOwner, const AActor* TargetActor)
{
	if (!PlayerOwner || !TargetActor || !OwnerPlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("UTargetingSystemComponent::UpdateTargetingCameraAnchorAndRotation - Invalid Access"));
		return;
	}
	
	const FVector PlayerLocation = PlayerOwner->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();
	FVector MidpointAnchorLocation = FMath::Lerp(PlayerLocation, TargetLocation, 0.5f);
	float Distance = FVector::Dist(PlayerLocation, TargetLocation);
	float DesiredRadius = Distance / 2.0f;

	if (bIsLockingOn)
	{
		SmoothedMidPoint = PlayerOwner->CameraAnchorComponent->GetComponentLocation();
		bIsLockingOn = false; 
	}

	// DrawCameraAnchor();

	float OffScreenInterpSpeed = CatchupToOffScreen(PlayerLocation, CatchupInterpSpeed);
	SmoothedMidPoint = FMath::VInterpTo(SmoothedMidPoint, MidpointAnchorLocation, GetWorld()->GetDeltaSeconds(), OffScreenInterpSpeed);

	// DrawDebugSphere(
	// GetWorld(),
	// MidpointAnchorLocation,
	// 15.0f,           
	// 12,                 
	// FColor::Yellow,        
	// false,              
	// -1.0f,              
	// 0                   
	// );

	if (IsValid(PlayerOwner->CameraAnchorComponent))
	{
		//TODO: Fix later
		// PlayerOwner->CameraAnchorComponent->SetWorldLocation(SmoothedMidPoint);
		
		PlayerOwner->CameraAnchorComponent->SetWorldLocation(MidpointAnchorLocation);

		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(SmoothedMidPoint, TargetLocation);
		const FRotator NewRotation = FMath::RInterpTo(PlayerOwner->CameraAnchorComponent->GetComponentRotation(), LookAtRotation, GetWorld()->GetDeltaSeconds(), 3.0f);
		PlayerOwner->CameraAnchorComponent->SetWorldRotation(NewRotation);
	}

	if (IsValid(PlayerOwner->SpringArmComponent))
	{
		const float TargetArmLength = DesiredRadius + 300.0f;

		UCameraSystemComponent* CSC = PlayerOwner->FindComponentByClass<UCameraSystemComponent>();
		
		// FCameraSystemParams CameraParams;
		// CameraParams.ShouldAdjustArmLength = true;
		// CameraParams.ShouldOverrideArmLength = true;
		// CameraParams.DeltaArmLength = TargetArmLength;
		//
		// CSC->HandleCameraSystemAdjustment(CameraParams);
		PlayerOwner->SpringArmComponent->TargetArmLength = FMath::FInterpTo(PlayerOwner->SpringArmComponent->TargetArmLength, TargetArmLength, GetWorld()->GetDeltaSeconds(), 3.0f);
	}

	float ControlRotationInterpSpeed = CompareDistanceToScreenAndGetInterpSpeed(PlayerOwner, TargetActor, ShouldUpdateControllerRotation);
	if (ShouldUpdateControllerRotation)
	{
		const FRotator ControlRotation = AddDistanceBasedAndInputOffset(TargetActor);
		FRotator FinalRotation = FMath::RInterpTo(OwnerPlayerController->GetControlRotation(), ControlRotation, GetWorld()->GetDeltaSeconds(), ControlRotationInterpSpeed * 5);
		OwnerPlayerController->SetControlRotation(FinalRotation);
	}
}

void UTargetingSystemComponent::TESTUpdateTargetingCameraAnchorAndRotation(APlayerCharacter* PlayerOwner, const AActor* TargetActor)
{
	
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

FRotator UTargetingSystemComponent::AddDistanceBasedAndInputOffset(const AActor* OtherActor) const
{
	if (!IsValid(OwnerPlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("UTargetSystemComponent::GetControlRotationOnTarget - OwnerPlayerController is not valid ..."))
		return FRotator::ZeroRotator;
	}

	const FRotator ControlRotation = OwnerPlayerController->GetControlRotation();

	const FVector CharacterLocation = OwnerActor->GetActorLocation();
	const FVector OtherActorLocation = OtherActor->GetActorLocation();

	const FRotator LookRotation = FindLookAtRotation(CharacterLocation,OtherActorLocation);
	float Yaw = LookRotation.Yaw;
	float Pitch = LookRotation.Pitch;

	const float DistanceToTarget = GetDistanceFromCharacter(OtherActor);
	float DesiredPitch = CalculateControlRotationOffset(DistanceToTarget, DistanceBasedMaxPitchOffset);
	float DesiredYaw = 0.0;
	
	// if(PlayerSideRelativeToActorOnScreen(OtherActor))
	// {
	// 	DesiredYaw = CalculateControlRotationOffset(DistanceToTarget, DistanceBasedMaxYawOffset);
	// }
	
	if(UViewportUtilityFunctionLibrary::PlayerSideRelativeToActorOnScreen(GetWorld(), OtherActor, PlayerCharacter, OwnerPlayerController))
	{
		DesiredYaw = CalculateControlRotationOffset(DistanceToTarget, DistanceBasedMaxYawOffset);
	}
	else
	{
		DesiredYaw = -CalculateControlRotationOffset(DistanceToTarget, DistanceBasedMaxYawOffset);
	}
	
	Pitch = Pitch + DesiredPitch;
	Yaw = Yaw + DesiredYaw;
		
	FRotator TargetRotation = FRotator(Pitch, Yaw, ControlRotation.Roll);
	if (EnableInputBasedOffset)
	{
		TargetRotation += CurrentCameraOffset;
	}
	return FMath::RInterpTo(ControlRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 9.0f);

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

void UTargetingSystemComponent::ControlCameraOffset(float DeltaTime)
{
	if (EnableInputBasedOffset && OwnerPlayerController && IsTargetLocked)
	{
		float YawInput = 0.0f;
		float PitchInput = 0.0f;

		OwnerPlayerController->GetInputMouseDelta(YawInput, PitchInput);

		const float InputScale = InputOffsetScale;
		YawInput *= InputScale;
		PitchInput *= InputScale;

		CurrentCameraOffset.Yaw += YawInput;
		CurrentCameraOffset.Pitch += PitchInput;

		CurrentCameraOffset.Yaw = FMath::Clamp(CurrentCameraOffset.Yaw, -InputBasedMaxYawOffset, InputBasedMaxYawOffset);
		CurrentCameraOffset.Pitch = FMath::Clamp(CurrentCameraOffset.Pitch, -InputBasedMaxPitchOffset, InputBasedMaxPitchOffset);

		const float DecayRate = InputOffsetDecayRate * DeltaTime;
		CurrentCameraOffset = FMath::RInterpTo(CurrentCameraOffset, FRotator::ZeroRotator, DeltaTime, DecayRate);
	}
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
	
	float DistanceFactor = FMath::Clamp((DistanceToScreenDifference), 0.0f, 10.0f);
	return FMath::Lerp(0.0f, 1.0f, DistanceFactor);
}

float UTargetingSystemComponent::GetDistanceFromCharacter(const AActor* OtherActor) const
{
	return OwnerActor->GetDistanceTo(OtherActor);
}

void UTargetingSystemComponent::SetOwnerActorRotation()
{
	FVector TargetLocation = LockedOnTargetActor->GetActorLocation();
	FRotator TargetRotation = FindLookAtRotation(OwnerActor->GetActorLocation(), TargetLocation);
	FRotator InterpRot = FMath::RInterpTo(OwnerActor->GetActorRotation(), FRotator(0.0, TargetRotation.Yaw, 0.0), GetWorld()->GetDeltaSeconds(), 10.0f);

	OwnerActor->SetActorRotation(InterpRot);
}

void UTargetingSystemComponent::EnableControlRotation(const bool InShouldControlRotation) const
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

/*
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
*/

void UTargetingSystemComponent::DisableMidPointControlRotation()
{
	if (!PlayerCharacter || !PlayerCharacter->CameraAnchorComponent) return;

	const FVector CurrentAnchorLocation = PlayerCharacter->CameraAnchorComponent->GetRelativeLocation();
	const FVector TargetAnchorLocation = PlayerCharacter->GetDefaultCameraAnchorLocation();
	const FVector NewLocation = FMath::VInterpTo(CurrentAnchorLocation, TargetAnchorLocation, GetWorld()->GetDeltaSeconds(), 2.0f);

	PlayerCharacter->CameraAnchorComponent->SetRelativeLocation(NewLocation);

	const FRotator CurrentAnchorRotation = PlayerCharacter->CameraAnchorComponent->GetRelativeRotation();
	const FRotator TargetAnchorRotation = PlayerCharacter->GetDefaultCameraAnchorRotation();
	const FRotator NewRotation = FMath::RInterpTo(CurrentAnchorRotation, TargetAnchorRotation, GetWorld()->GetDeltaSeconds(), 2.0f);

	PlayerCharacter->CameraAnchorComponent->SetRelativeRotation(NewRotation);
	
	const float CurrentTargetArmLength = PlayerCharacter->SpringArmComponent->TargetArmLength;
	PlayerCharacter->SpringArmComponent->TargetArmLength = FMath::FInterpTo(CurrentTargetArmLength, 400, GetWorld()->GetDeltaSeconds(), 2.0f);
	
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

AActor* UTargetingSystemComponent::FindNearestTargetToActor(TArray<AActor*> Actors) const
{
	TArray<AActor*> ActorsHit;

	for (AActor* Actor : Actors)
	{
		TArray<AActor*> ActorsToIgnore;
		const bool bHit = LineTraceForActor(Actor, ActorsToIgnore);
		if (bHit && UViewportUtilityFunctionLibrary::IsInViewport(GetWorld(), Actor, OwnerPlayerController))
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
		if (bHit && UViewportUtilityFunctionLibrary::IsInViewport(GetWorld(), Actor, OwnerPlayerController))
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

float UTargetingSystemComponent::FindDistanceFromCenterOfViewport(const AActor* TargetActor) const
{
	FVector ActorLocation = TargetActor->GetActorLocation();
	FVector2D ScreenPosition;
	
	OwnerPlayerController->ProjectWorldLocationToScreen(ActorLocation, ScreenPosition);
	
	FVector2D CenterOfScreen = UViewportUtilityFunctionLibrary::FindCenterOfViewPort(GetWorld());
	
	return FVector2D::Distance(CenterOfScreen, ScreenPosition);
	
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


AActor* UTargetingSystemComponent::GetLockedOnTargetActor() const
{
	return LockedOnTargetActor;
}


bool UTargetingSystemComponent::IsLocked() const
{
	return IsTargetLocked && LockedOnTargetActor;
}

bool UTargetingSystemComponent::GetTargetLockedStatus()
{
	return IsTargetLocked;
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
	LockedOnTargetActor = nullptr;
}
