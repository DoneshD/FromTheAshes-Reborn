#include "Camera/CameraSystemComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraParamsDataAsset.h"
#include "GameFramework/SpringArmComponent.h"
#include "HelperFunctionLibraries/ViewportUtilityFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/FTAPlayerCameraManger.h"
#include "TargetingSystem/TargetingSystemComponent.h"

UCameraSystemComponent::UCameraSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	BaseArmLengthFromTargetingSystem = 400.0f;
	ArmLengthOffset = 0.0f;
	ArmLengthLerpSpeed = 0.0f;
	NewSpringArmLength = 0.0f;

	CameraBaseFOV = 90.0f;
	CameraFOVOffset = 0.0f;
	CameraFOVLerpSpeed = 2.0f;

	CameraAnchorInterpSpeed = 5.0f;

	UseWorldTransform = false;

	NewCameraAnchorLocation = FVector::ZeroVector;
	NewCameraAnchorRotation = FRotator::ZeroRotator;

	AnchorTransformLocation = FVector::ZeroVector;
	AnchorTransformRotation = FRotator::ZeroRotator;
	
	CurrentCameraOffset = FRotator::ZeroRotator;

	
}

void UCameraSystemComponent::BeginPlay()
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

	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());

	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCharacter is null"));
		return;
	}

	SetupLocalPlayerController();

	CameraAnchorComponent = PlayerCharacter->CameraAnchorComponent;

	if (!IsValid(CameraAnchorComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("CameraAnchorComponent is null"));
		return;
	}

	CameraComponent = PlayerCharacter->CameraComponent;

	if (!CameraComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CameraComponent is null"));
		return;
	}
	
	SpringArmComponent = PlayerCharacter->SpringArmComponent;
	
	if (!SpringArmComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("SpringArmComponent is null"));
		return;
	}
	AddCameraParameters(NeutralCameraStateParams);
}

void UCameraSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*for (const TObjectPtr<UCameraParamsDataAsset>& Params : CameraParamsArray)
	{
		UE_LOG(LogTemp, Warning, TEXT("CameraParams: %s"),
			Params ? *Params->GetName() : TEXT("NULL"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Size: %d"), CameraParamsArray.Num());*/

	if (SpringArmComponent)
	{
		
		float CurrentLength = SpringArmComponent->TargetArmLength;
		NewSpringArmLength = ResolveSpringArmLength();
		float TargetLength = NewSpringArmLength + ArmLengthOffset;
	
		if (!FMath::IsNearlyEqual(CurrentLength, TargetLength, 0.1f))
		{
			float InterpolatedLength = FMath::FInterpTo(CurrentLength, TargetLength, DeltaTime, 3.0f);
			SpringArmComponent->TargetArmLength = InterpolatedLength;
	
			// UE_LOG(LogTemp, Warning, TEXT("Spring Arm Lerp Debug -> CurrentLength: %.2f, Base: %.2f, Offset: %.2f, FinalTarget: %.2f"),
			// 	CurrentLength, BaseArmLengthFromTargetingSystem, ArmLengthOffset, InterpolatedLength);
		}
		
	}
	
	if (CameraComponent)
	{
		float CurrentFOV = CameraComponent->FieldOfView;
		float FinalTargetFOV = CameraBaseFOV + CameraFOVOffset;
		
		FinalTargetFOV = FMath::Clamp(FinalTargetFOV, 80.0, 100.0f);
	
		if (!FMath::IsNearlyEqual(CurrentFOV, FinalTargetFOV, 0.1f))
		{
			const float InterpolatedFOV = FMath::InterpEaseOut(CurrentFOV, FinalTargetFOV, DeltaTime * CameraFOVLerpSpeed, 2.0);
			CameraComponent->SetFieldOfView(InterpolatedFOV);
	
			// UE_LOG(LogTemp, Warning, TEXT("FOV Lerp Debug -> Current: %.2f, Base: %.2f, Offset: %.2f, Target: %.2f, Interpolated: %.2f, DeltaTime: %.2f, Speed: %.2f"),
			// 	CurrentFOV, CameraBaseFOV, CameraFOVOffset, FinalTargetFOV, InterpolatedFOV, DeltaTime, CameraFOVLerpSpeed);
		}
	}

	if (CameraAnchorComponent)
	{
		if(UseWorldTransform)
		{
			FVector CurrentAnchorLocation = AnchorTransformLocation;
	
			if (!CurrentAnchorLocation.Equals(NewCameraAnchorLocation, 0.1f))
			{
				// FVector InterpolatedLocation = FMath::VInterpTo(CurrentAnchorLocation, NewCameraAnchorLocation, GetWorld()->GetDeltaSeconds(), CameraAnchorInterpSpeed);
				FVector InterpolatedLocation = FMath::Lerp(CurrentAnchorLocation, NewCameraAnchorLocation, 0.5f);
				CameraAnchorComponent->SetWorldLocation(InterpolatedLocation);
			}
	
			FRotator CurrentAnchorRotation = AnchorTransformRotation;
	
			if (!CurrentAnchorRotation.Equals(NewCameraAnchorRotation, 0.1f))
			{
				FRotator InterpolatedRotation = FMath::RInterpTo(CurrentAnchorRotation, NewCameraAnchorRotation, GetWorld()->GetDeltaSeconds(), CameraAnchorInterpSpeed);
				CameraAnchorComponent->SetWorldRotation(InterpolatedRotation);
			}
		}
		if(!UseWorldTransform)
		{
			FVector CurrentAnchorLocation = AnchorTransformLocation;
	
			if (!CurrentAnchorLocation.Equals(NewCameraAnchorLocation, 0.1f))
			{
				FVector InterpolatedLocation = FMath::VInterpTo(CurrentAnchorLocation, NewCameraAnchorLocation, GetWorld()->GetDeltaSeconds(), CameraAnchorInterpSpeed);
				CameraAnchorComponent->SetRelativeLocation(InterpolatedLocation);
			}
	
			FRotator CurrentAnchorRotation = AnchorTransformRotation;
	
			if (!CurrentAnchorRotation.Equals(NewCameraAnchorRotation, 0.1f))
			{
				FRotator InterpolatedRotation = FMath::RInterpTo(CurrentAnchorRotation, NewCameraAnchorRotation, GetWorld()->GetDeltaSeconds(), CameraAnchorInterpSpeed);
				CameraAnchorComponent->SetRelativeRotation(InterpolatedRotation);
			}
		}
	}


}

float UCameraSystemComponent::ResolveSpringArmLength()
{
	float TargetSpringArmLength = BaseArmLengthFromTargetingSystem;

	TArray<TObjectPtr<UCameraParamsDataAsset>> Sorted = CameraParamsArray;
	Sorted.Sort([](const TObjectPtr<UCameraParamsDataAsset>& A, const TObjectPtr<UCameraParamsDataAsset>& B)
	{
		return A->SpringArmParams.Priority > B->SpringArmParams.Priority;
	});

	for(const TObjectPtr<UCameraParamsDataAsset>& Params1 : Sorted)
	{
		if(!Params1->SpringArmParams.ShouldAdjustArmLength)
		{
			continue;
		}
		
		if(Params1->SpringArmParams.CameraOperation == ECameraOperation::Set)
		{
			TargetSpringArmLength = Params1->SpringArmParams.Value;
			break;
		}

		for(const TObjectPtr<UCameraParamsDataAsset>& Params2 : Sorted)
		{
			if(!Params1->SpringArmParams.ShouldAdjustArmLength)
			{
				if(Params1->SpringArmParams.CameraOperation == ECameraOperation::Additive)
				{
					TargetSpringArmLength += Params1->SpringArmParams.Value;
					break;
				}
			}
		}
		
	}
	return TargetSpringArmLength;

}

void UCameraSystemComponent::HandleSpringArmAdjustment(float InDeltaLength, float InInterpSpeed, bool InShouldOverride, bool InShouldResetOffset)
{
	/*if (SpringArmComponent)
	{
		// BaseArmLengthFromTargetingSystem = InDeltaLength;
		NewSpringArmLength = InDeltaLength;
		ArmLengthLerpSpeed = InInterpSpeed;
	}*/
}

void UCameraSystemComponent::HandleCameraComponentAdjustment(float InDeltaFOV, float InInterpSpeed, bool InShouldOverride, bool InShouldResetOffset)
{
	if (CameraComponent)
	{
		if (InShouldOverride)
		{
			CameraBaseFOV = InDeltaFOV;

			if (InShouldResetOffset)
			{
				CameraFOVOffset = 0.0f;
			}
		}
		else
		{
			CameraFOVOffset += InDeltaFOV;
		}

		CameraFOVLerpSpeed = InInterpSpeed;
	}
}

void UCameraSystemComponent::HandleCameraAnchorAdjustment(FVector InLocation, FRotator InRotation, bool ShouldUseWorldTransform, bool InShouldOverride, bool InShouldResetOffset, float InInterpSpeed)
{
	if (CameraAnchorComponent)
	{
		if(ShouldUseWorldTransform)
		{
			UseWorldTransform = true;
			AnchorTransformLocation = PlayerCharacter->CameraAnchorComponent->GetComponentLocation();
			AnchorTransformRotation = PlayerCharacter->CameraAnchorComponent->GetComponentRotation();
		}
		else
		{
			UseWorldTransform = false;
			AnchorTransformLocation = PlayerCharacter->CameraAnchorComponent->GetRelativeLocation();
			AnchorTransformRotation = PlayerCharacter->CameraAnchorComponent->GetRelativeRotation();
		}
		if (InShouldOverride)
		{
			NewCameraAnchorLocation = InLocation;
			NewCameraAnchorRotation = InRotation;
		}
		CameraAnchorInterpSpeed = InInterpSpeed;
	}
}

void UCameraSystemComponent::AddCameraParameters(UCameraParamsDataAsset* CameraParams)
{
	CameraParamsArray.AddUnique(CameraParams);
}

void UCameraSystemComponent::HandleCameraSystemAdjustment(UCameraParamsDataAsset* Params)
{
	// if(Params->SpringArmParams.ShouldAdjustArmLength)
	// {
	// 	HandleSpringArmAdjustment(Params->SpringArmParams.DeltaArmLength,
	// 		Params->SpringArmParams.DeltaArmLengthInterpSpeed,
	// 		Params->SpringArmParams.ShouldOverrideArmLength,
	// 		Params->SpringArmParams.ShouldResetOffset);
	// }

	if(Params->CameraComponentParams.ShouldAdjustFOV)
	{
		HandleCameraComponentAdjustment(Params->CameraComponentParams.DeltaFOV,
			Params->CameraComponentParams.DeltaFOVInterpSpeed,
			Params->CameraComponentParams.ShouldOverrideFOV,
			Params->CameraComponentParams.ShouldResetFOVOffset);
	}

	if(Params->CameraAnchorParams.ShouldAdjustAnchor)
	{
		HandleCameraAnchorAdjustment(Params->CameraAnchorParams.NewAnchorLocation,
			Params->CameraAnchorParams.NewAnchorRotation,
			Params->CameraAnchorParams.ShouldUseWorldTransform, 
			Params->CameraAnchorParams.ShouldOverrideAnchor,
			Params->CameraAnchorParams.ShouldResetAnchorOffset,
			Params->CameraAnchorParams.DeltaAnchorInterpSpeed);
	}
}

void UCameraSystemComponent::RemoveCameraParameters(UCameraParamsDataAsset* CameraParams)
{
	CameraParamsArray.Remove(CameraParams);
}

void UCameraSystemComponent::NeutralCameraState(TObjectPtr<UCameraParamsDataAsset> CameraParams)
{
	
	const FVector CurrentAnchorLocation = PlayerCharacter->CameraAnchorComponent->GetRelativeLocation();
	const FVector TargetAnchorLocation = CameraParams->CameraAnchorParams.TargetTransform.GetLocation();
	const FVector NewLocation = FMath::VInterpTo(CurrentAnchorLocation, TargetAnchorLocation, GetWorld()->GetDeltaSeconds(), 2.0f);
	
	PlayerCharacter->CameraAnchorComponent->SetRelativeLocation(NewLocation);
	
	const FRotator CurrentAnchorRotation = PlayerCharacter->CameraAnchorComponent->GetRelativeRotation();
	const FRotator TargetAnchorRotation = CameraParams->CameraAnchorParams.TargetTransform.Rotator();
	// const FRotator NewRotation = FMath::RInterpTo(CurrentAnchorRotation, TargetAnchorRotation, GetWorld()->GetDeltaSeconds(), 2.0f);
	
	// PlayerCharacter->CameraAnchorComponent->SetRelativeRotation(NewRotation);
	
	
	const float CurrentTargetArmLength = PlayerCharacter->SpringArmComponent->TargetArmLength;
	PlayerCharacter->SpringArmComponent->TargetArmLength = FMath::FInterpTo(CurrentTargetArmLength, 400, GetWorld()->GetDeltaSeconds(), 2.0f);
	
	HandleCameraAnchorAdjustment(TargetAnchorLocation, TargetAnchorRotation, false, true, true, 2.0f);
	// HandleSpringArmAdjustment(PlayerCharacter->DefaultSpringArmLength, 3.0, true, true);

}

void UCameraSystemComponent::ControlCameraOffset(float DeltaTime, TObjectPtr<UCameraParamsDataAsset> CameraParams)
{
	if (CameraParams->InputOffsetInfo.EnableInputBasedOffset && OwnerPlayerController && TargetingSystemComponent->IsTargetLocked)
	{
		float YawInput = 0.0f;
		float PitchInput = 0.0f;

		OwnerPlayerController->GetInputMouseDelta(YawInput, PitchInput);
			
		const float InputScale = CameraParams->InputOffsetInfo.InputOffsetScale;

		YawInput *= InputScale;
		PitchInput *= InputScale;
			
		CurrentCameraOffset.Yaw += YawInput;
		CurrentCameraOffset.Pitch += PitchInput;
			
		CurrentCameraOffset.Yaw = FMath::Clamp(CurrentCameraOffset.Yaw, -CameraParams->InputOffsetInfo.InputBasedMaxYawOffset, CameraParams->InputOffsetInfo.InputBasedMaxYawOffset);
		CurrentCameraOffset.Pitch = FMath::Clamp(CurrentCameraOffset.Pitch, -CameraParams->InputOffsetInfo.InputBasedMaxPitchOffset, CameraParams->InputOffsetInfo.InputBasedMaxPitchOffset);

		const float DecayRate = CameraParams->InputOffsetInfo.InputOffsetDecayRate * DeltaTime;

		CurrentCameraOffset = FMath::RInterpTo(CurrentCameraOffset, FRotator::ZeroRotator, DeltaTime, DecayRate);
	}
}

void UCameraSystemComponent::SetupLocalPlayerController()
{
	if (!IsValid(OwnerPawn))
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] TargetSystemComponent: Component is meant to be added to Pawn only ..."), *GetName());
		return;
	}

	OwnerPlayerController = Cast<APlayerController>(OwnerPawn->GetController());

	if (!IsValid(OwnerPlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("UCameraSystemComponent::SetupLocalPlayerController() - OwnerPlayerController is invalid"));
		return;
	}

	if (!PlayerCharacter || !PlayerCharacter->CameraAnchorComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UCameraSystemComponent::SetupLocalPlayerController() - Missing CameraAnchorComponent"));
		return;
	}

	
	FTAPlayerCameraManger = Cast<AFTAPlayerCameraManger>(OwnerPlayerController->PlayerCameraManager);
	
	if (!IsValid(FTAPlayerCameraManger))
	{
		UE_LOG(LogTemp, Error, TEXT("UCameraSystemComponent::SetupLocalPlayerController() - FTAPlayerCameraManger is invalid"));
	}
	
	TargetingSystemComponent = PlayerCharacter->FindComponentByClass<UTargetingSystemComponent>();

	if (!IsValid(TargetingSystemComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("UCameraSystemComponent::SetupLocalPlayerController() - TargetingSystemComponent is invalid"));
	}

}

void UCameraSystemComponent::DrawCameraAnchor()
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

void UCameraSystemComponent::UpdateTargetingCameraAnchorAndRotation(APlayerCharacter* PlayerOwner, const AActor* TargetActor, float DeltaTime, TObjectPtr<UCameraParamsDataAsset> CameraParams)
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

	if (TargetingSystemComponent->bIsLockingOn)
	{
		SmoothedMidPoint = PlayerOwner->CameraAnchorComponent->GetComponentLocation();
		TargetingSystemComponent->bIsLockingOn = false; 
	}

	DrawCameraAnchor();

	float OffScreenInterpSpeed = CatchupToOffScreen(PlayerLocation, CameraParams->CatchupInterpSpeed, CameraParams);
	SmoothedMidPoint = FMath::VInterpTo(SmoothedMidPoint, MidpointAnchorLocation, DeltaTime, OffScreenInterpSpeed);

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

	if (IsValid(PlayerOwner->CameraAnchorComponent))
	{
		//TODO: Fix later
		// PlayerOwner->TargetCameraAnchor->SetWorldLocation(SmoothedMidPoint);
		
		// PlayerOwner->CameraAnchorComponent->SetWorldLocation(MidpointAnchorLocation);

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(SmoothedMidPoint, TargetLocation);
		
		// const FRotator NewRotation = FMath::RInterpTo(PlayerOwner->CameraAnchorComponent->GetComponentRotation(), LookAtRotation, GetWorld()->GetDeltaSeconds(), 3.0f);
		// PlayerOwner->CameraAnchorComponent->SetWorldRotation(NewRotation);

		DrawDebugSphere(
			GetWorld(),
			SmoothedMidPoint,
			15.0f,           
			12,                 
			FColor::Blue,        
			false,              
			-1.0f,              
			0                   
			);
		HandleCameraAnchorAdjustment(SmoothedMidPoint, LookAtRotation, true, true, true, OffScreenInterpSpeed);
		
	}

	if (IsValid(PlayerOwner->SpringArmComponent))
	{
		float TargetArmLength = DesiredRadius + 300.0f;
		HandleSpringArmAdjustment(600, 3.0, true, true);
	}

	float ControlRotationInterpSpeed = CompareDistanceToScreenAndGetInterpSpeed(PlayerOwner, TargetActor, ShouldUpdateControllerRotation);
	if (ShouldUpdateControllerRotation)
	{
		FRotator ControlRotation = AddDistanceBasedAndInputOffset(TargetActor, CameraParams);
		FRotator FinalRotation = FMath::RInterpTo(OwnerPlayerController->GetControlRotation(), ControlRotation, DeltaTime, ControlRotationInterpSpeed);
		OwnerPlayerController->SetControlRotation(FinalRotation);
	}
}

float UCameraSystemComponent::CatchupToOffScreen(const FVector& PlayerLocation, float& InInterpSpeed, TObjectPtr<UCameraParamsDataAsset> CameraParams)
{
	float InterpSpeed = InInterpSpeed;
	const float ScreenEdgeCatchupThreshold = .35f;

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
			InterpSpeed = CameraParams->CatchupInterpSpeed;
		}
	}
	return InterpSpeed;
}

float UCameraSystemComponent::CompareDistanceToScreenAndGetInterpSpeed(APlayerCharacter* PlayerOwner,
	const AActor* TargetActor, bool& InShouldUpdateControlRotation)
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

float UCameraSystemComponent::GetWorldDistanceFromCamera(APlayerController* PlayerController, const AActor* ActorToCheck)
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

FRotator UCameraSystemComponent::AddDistanceBasedAndInputOffset(const AActor* OtherActor, TObjectPtr<UCameraParamsDataAsset> CameraParams) const
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
	float DesiredPitch = CalculateControlRotationOffset(DistanceToTarget, CameraParams->DistanceBasedMaxPitchOffset);
	float DesiredYaw = 0.0;
	
	// if(PlayerSideRelativeToActorOnScreen(OtherActor))
	// {
	// 	DesiredYaw = CalculateControlRotationOffset(DistanceToTarget, DistanceBasedMaxYawOffset);
	// }
	
	if(UViewportUtilityFunctionLibrary::PlayerSideRelativeToActorOnScreen(GetWorld(), OtherActor, PlayerCharacter, OwnerPlayerController))
	{
		DesiredYaw = CalculateControlRotationOffset(DistanceToTarget, CameraParams->DistanceBasedMaxYawOffset);
	}
	else
	{
		DesiredYaw = -CalculateControlRotationOffset(DistanceToTarget, CameraParams->DistanceBasedMaxYawOffset);
	}
	
	Pitch = Pitch + DesiredPitch;
	Yaw = Yaw + DesiredYaw;
		
	FRotator TargetRotation = FRotator(Pitch, Yaw, ControlRotation.Roll);
	if (CameraParams->InputOffsetInfo.EnableInputBasedOffset)
	{
		TargetRotation += CurrentCameraOffset;
	}
	return FMath::RInterpTo(ControlRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 9.0f);
}

FRotator UCameraSystemComponent::FindLookAtRotation(const FVector Start, const FVector Target)
{
	return FRotationMatrix::MakeFromX(Target - Start).Rotator();
	
}

float UCameraSystemComponent::GetDistanceFromCharacter(const AActor* OtherActor) const
{
	return OwnerActor->GetDistanceTo(OtherActor);
}

float UCameraSystemComponent::CalculateControlRotationOffset(float Distance, float MaxOffset) const
{
	if (Distance > TargetingSystemComponent->MaxDistance)
	{
		return 0.0f;
	}

	float DistanceFactor = 1.0f - FMath::Clamp((Distance - TargetingSystemComponent->MinDistance) / (TargetingSystemComponent->MaxDistance - TargetingSystemComponent->MinDistance), 0.0f, 1.0f);
	return FMath::Lerp(0.0f, MaxOffset, DistanceFactor);
}
