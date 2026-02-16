#include "TargetingSystem/TargetingSystemComponent.h"

#include "AbilitySystemGlobals.h"
#include "Camera/CameraSystemComponent.h"
#include "EngineUtils.h"
#include "TargetingSystem/TargetingSystemTargetableInterface.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
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

	TargetableActors = APawn::StaticClass();
	TargetableCollisionChannel = ECollisionChannel::ECC_Pawn;
	
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
		CameraSystemComponent->NeutralCameraState();
	}
	else
	{
		
		CameraSystemComponent->ControlCameraOffset(DeltaTime, CameraParameters);
		CameraSystemComponent->UpdateTargetingCameraAnchorAndRotation(PlayerCharacter, LockedOnTargetActor, DeltaTime, CameraParameters);
		
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
	
	if (OwnerActor->GetDistanceTo((LockedOnTargetActor)) > MinimumDistanceToEnable)
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
			CameraSystemComponent->OwnerPlayerController->SetIgnoreLookInput(false);;
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
		TargetLockedOnWidgetComponent->SetOwnerPlayer(CameraSystemComponent->OwnerPlayerController->GetLocalPlayer());
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

	if (!PlayerCharacter || !PlayerCharacter->CameraAnchorComponent) return;
	

	CameraSystemComponent = PlayerCharacter->FindComponentByClass<UCameraSystemComponent>();
	
	if (!CameraSystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UTargetingSystemComponent::SetupLocalPlayerController() - CameraSystemComponent is NULL"))
		return;
	}

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
	TArray<AActor*> ActorsWithInRange;

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
	
	for (AActor* HitActor : ActorsHit)
	{
		const float Distance = OwnerActor->GetDistanceTo(HitActor);

		if (Distance < MinimumDistanceToEnable)
		{
			ClosestDistance = Distance;
			// Target = Actor;
			ActorsWithInRange.Add(HitActor);
		}
	}
	Target = FindNearestTargetToCenterViewport(ActorsWithInRange);
	
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
	
	// if (IsTargetLocked)
	// {
	// 	UE_LOG(LogTemp, Display, TEXT("UTargetingSystemComponent::TargetActor"));
	// 	TargetLockOff();
	// 	IsSuccess = false;
	// 	return nullptr;
	// }
	
	const TArray<AActor*> Actors = GetAllActorsOfClass(TargetableActors);
	LockedOnTargetActor = FindNearestTargetToActor(Actors);
	// LockedOnTargetActor = FindNearestTargetToCenterViewport(Actors);

	if(LockedOnTargetActor)
	{
		TargetLockOn(LockedOnTargetActor);
	
		IsSuccess = true;
		FTAPlayerCameraManger->ViewPitchMax = 10;
		return LockedOnTargetActor;
	}
	TargetLockOff();
	IsSuccess = false;
	return nullptr;
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
			CameraSystemComponent->OwnerPlayerController->ResetIgnoreLookInput();
			OwnerPlayerController->ResetIgnoreLookInput();
		}

		if (OnTargetLockedOff.IsBound())
		{
			OnTargetLockedOff.Broadcast(LockedOnTargetActor);
		}
	}
	if(FTAPlayerCameraManger)
	{
		FTAPlayerCameraManger->ViewPitchMax = 30.0f;
	}
	LockedOnTargetActor = nullptr;
}
