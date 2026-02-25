#include "Camera/CameraSystemComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "MaterialHLSLTree.h"
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

	CameraBaseFOV = 90.0f;
	CameraFOVOffset = 0.0f;
	CameraFOVLerpSpeed = 2.0f;
	
	
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
		float TestFloat = ResolveSpringArmLength();
	
		if (!FMath::IsNearlyEqual(CurrentLength, CurrentCameraStateParams->SpringArmParams.ArmLength.Value, 0.1f))
		{
			float InterpolatedLength = FMath::FInterpTo(CurrentLength, CurrentCameraStateParams->SpringArmParams.ArmLength.Value, DeltaTime, CurrentCameraStateParams->SpringArmParams.ArmLength.MetaData.InLerpSpeedFloat);
			SpringArmComponent->TargetArmLength = InterpolatedLength;
			
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
		ResolveCameraAnchorTransform();
		
		if(CurrentCameraStateParams->CameraAnchorParams.UseWorldTransform)
		{
			FVector CurrentAnchorLocation = CameraAnchorComponent->GetComponentLocation();
	
			if (!CurrentAnchorLocation.Equals(CurrentCameraStateParams->CameraAnchorParams.TargetLocation.Value, 0.1f))
			{
				// FVector InterpolatedLocation = FMath::VInterpTo(CurrentAnchorLocation, NewCameraAnchorLocation, GetWorld()->GetDeltaSeconds(), CameraAnchorInterpSpeed);
				FVector InterpolatedLocation = FMath::Lerp(CurrentAnchorLocation, CurrentCameraStateParams->CameraAnchorParams.TargetLocation.Value, 0.5f);
				CameraAnchorComponent->SetWorldLocation(InterpolatedLocation);
			}
	
			FRotator CurrentAnchorRotation = CameraAnchorComponent->GetComponentRotation();
	
			if (!CurrentAnchorRotation.Equals(CurrentCameraStateParams->CameraAnchorParams.TargetRotation.Value, 0.1f))
			{
				FRotator InterpolatedRotation = FMath::RInterpTo(CurrentAnchorRotation, CurrentCameraStateParams->CameraAnchorParams.TargetRotation.Value, GetWorld()->GetDeltaSeconds(), CurrentCameraStateParams->CameraAnchorParams.TargetRotation.MetaData.InLerpSpeedFloat);
				CameraAnchorComponent->SetWorldRotation(InterpolatedRotation);
			}
		}
		if(!CurrentCameraStateParams->CameraAnchorParams.UseWorldTransform)
		{
			FVector CurrentAnchorLocation = CameraAnchorComponent->GetRelativeLocation();
	
			if (!CurrentAnchorLocation.Equals(CurrentCameraStateParams->CameraAnchorParams.TargetLocation.Value, 0.1f))
			{
				FVector InterpolatedLocation = FMath::VInterpTo(CurrentAnchorLocation, CurrentCameraStateParams->CameraAnchorParams.TargetLocation.Value, GetWorld()->GetDeltaSeconds(), CurrentCameraStateParams->CameraAnchorParams.TargetLocation.MetaData.InLerpSpeedFloat);
				CameraAnchorComponent->SetRelativeLocation(InterpolatedLocation);
			}
	
			FRotator CurrentAnchorRotation = CameraAnchorComponent->GetRelativeRotation();
	
			if (!CurrentAnchorRotation.Equals(CurrentCameraStateParams->CameraAnchorParams.TargetRotation.Value, 0.1f))
			{
				FRotator InterpolatedRotation = FMath::RInterpTo(CurrentAnchorRotation, CurrentCameraStateParams->CameraAnchorParams.TargetRotation.Value, GetWorld()->GetDeltaSeconds(),CurrentCameraStateParams->CameraAnchorParams.TargetRotation.MetaData.InLerpSpeedFloat);
				CameraAnchorComponent->SetRelativeRotation(InterpolatedRotation);
			}
		}
	}

	if(OwnerPlayerController)
	{
		ResolveControlRotation();
		FRotator FinalRotation = FMath::RInterpTo(OwnerPlayerController->GetControlRotation(), CurrentCameraStateParams->ControlRotationParams.TargetControlRotation, DeltaTime, 8.0f);
		OwnerPlayerController->SetControlRotation(FinalRotation);
	}
}

float UCameraSystemComponent::ResolveSpringArmLength()
{
	TArray<TObjectPtr<UCameraParamsDataAsset>> Sorted = CameraParamsArray;
	Sorted.Sort([](const TObjectPtr<UCameraParamsDataAsset>& A, const TObjectPtr<UCameraParamsDataAsset>& B)
	{
		return A->SpringArmParams.Priority > B->SpringArmParams.Priority;
	});
	

	for(const TObjectPtr<UCameraParamsDataAsset>& Params : Sorted)
	{
		if(!Params->SpringArmParams.ShouldAdjust)
		{
			continue;
		}
		
		if(Params->SpringArmParams.CameraOperation == ECameraOperation::Set)
		{
			CurrentCameraStateParams->SpringArmParams.ArmLength = Params->SpringArmParams.ArmLength;
			break;
		}
	}

	for(const TObjectPtr<UCameraParamsDataAsset>& Params : Sorted)
	{
		if(!Params->SpringArmParams.ShouldAdjust)
		{
			continue;
		}
		
		if(Params->SpringArmParams.CameraOperation == ECameraOperation::LockOn)
		{
			CurrentCameraStateParams->SpringArmParams.ArmLength = TargetingSystemComponent->CameraParameters->SpringArmParams.ArmLength;
			
			break;
		}
	}

	for(const TObjectPtr<UCameraParamsDataAsset>& Params : Sorted)
	{
		if(!Params->SpringArmParams.ShouldAdjust)
		{
			continue;
		}
		
		if(Params->SpringArmParams.CameraOperation == ECameraOperation::Override)
		{
			
			break;
		}
	}

	for(const TObjectPtr<UCameraParamsDataAsset>& Params : Sorted)
	{
		if(!Params->SpringArmParams.ShouldAdjust)
		{
			continue;
		}
		
		if(Params->SpringArmParams.CameraOperation == ECameraOperation::Additive)
		{
			
			break;
		}
		
	}
	
	return 0.0;

}

void UCameraSystemComponent::ResolveCameraAnchorTransform()
{
	TArray<TObjectPtr<UCameraParamsDataAsset>> Sorted = CameraParamsArray;
	Sorted.Sort([](const TObjectPtr<UCameraParamsDataAsset>& A, const TObjectPtr<UCameraParamsDataAsset>& B)
	{
		return A->CameraAnchorParams.Priority > B->CameraAnchorParams.Priority;
	});
	

	for(const TObjectPtr<UCameraParamsDataAsset>& Params : Sorted)
	{
		if(!Params->CameraAnchorParams.ShouldAdjust)
		{
			continue;
		}
		
		if(Params->CameraAnchorParams.CameraOperation == ECameraOperation::Set)
		{
			// CurrentCameraStateParams->CameraAnchorParams.TargetLocation = Params->CameraAnchorParams.TargetLocation;
			// CurrentCameraStateParams->CameraAnchorParams.TargetRotation = Params->CameraAnchorParams.TargetRotation;
			// CurrentCameraStateParams->CameraAnchorParams.UseWorldTransform = Params->CameraAnchorParams.UseWorldTransform;
			// CurrentCameraStateParams->CameraAnchorParams.InLerpSpeedFloat = Params->CameraAnchorParams.InLerpSpeedFloat;

			CurrentCameraStateParams->CameraAnchorParams = Params->CameraAnchorParams;
			
			break;
		}
	}

	for(const TObjectPtr<UCameraParamsDataAsset>& Params : Sorted)
	{
		if(!Params->CameraAnchorParams.ShouldAdjust)
		{
			continue;
		}
		
		if(Params->CameraAnchorParams.CameraOperation == ECameraOperation::LockOn)
		{
			// CurrentCameraStateParams->CameraAnchorParams.TargetLocation = TargetingSystemComponent->CameraParameters->CameraAnchorParams.TargetLocation;
			// CurrentCameraStateParams->CameraAnchorParams.UseWorldTransform = TargetingSystemComponent->CameraParameters->CameraAnchorParams.UseWorldTransform;
			// CurrentCameraStateParams->CameraAnchorParams.InLerpSpeedFloat = TargetingSystemComponent->CameraParameters->CameraAnchorParams.InLerpSpeedFloat;

			CurrentCameraStateParams->CameraAnchorParams = TargetingSystemComponent->CameraParameters->CameraAnchorParams;
			
			break;
		}
	}

	for(const TObjectPtr<UCameraParamsDataAsset>& Params : Sorted)
	{
		if(!Params->CameraAnchorParams.ShouldAdjust)
		{
			continue;
		}
		
		if(Params->CameraAnchorParams.CameraOperation == ECameraOperation::Override)
		{
			
			break;
		}
	}

	for(const TObjectPtr<UCameraParamsDataAsset>& Params : Sorted)
	{
		if(!Params->CameraAnchorParams.ShouldAdjust)
		{
			continue;
		}
		
		if(Params->CameraAnchorParams.CameraOperation == ECameraOperation::Additive)
		{
			break;
		}
	}
}

void UCameraSystemComponent::ResolveControlRotation()
{
	TArray<TObjectPtr<UCameraParamsDataAsset>> Sorted = CameraParamsArray;
	Sorted.Sort([](const TObjectPtr<UCameraParamsDataAsset>& A, const TObjectPtr<UCameraParamsDataAsset>& B)
	{
		return A->CameraAnchorParams.Priority > B->CameraAnchorParams.Priority;
	});

	for(const TObjectPtr<UCameraParamsDataAsset>& Params : Sorted)
	{
		if(!Params->CameraAnchorParams.ShouldAdjust)
		{
			continue;
		}
		
		if(Params->CameraAnchorParams.CameraOperation == ECameraOperation::Set)
		{
			CurrentCameraStateParams->ControlRotationParams.TargetControlRotation = OwnerPlayerController->GetControlRotation();
			break;
		}
	}

	for(const TObjectPtr<UCameraParamsDataAsset>& Params : Sorted)
	{
		if(!Params->CameraAnchorParams.ShouldAdjust)
		{
			continue;
		}
		
		if(Params->CameraAnchorParams.CameraOperation == ECameraOperation::LockOn)
		{
			CurrentCameraStateParams->ControlRotationParams.TargetControlRotation = TargetingSystemComponent->CameraParameters->ControlRotationParams.TargetControlRotation;
			
			break;
		}
	}

	for(const TObjectPtr<UCameraParamsDataAsset>& Params : Sorted)
	{
		if(!Params->CameraAnchorParams.ShouldAdjust)
		{
			continue;
		}
		
		if(Params->CameraAnchorParams.CameraOperation == ECameraOperation::Override)
		{
			break;
		}
	}

	for(const TObjectPtr<UCameraParamsDataAsset>& Params : Sorted)
	{
		if(!Params->CameraAnchorParams.ShouldAdjust)
		{
			continue;
		}
		
		if(Params->CameraAnchorParams.CameraOperation == ECameraOperation::Additive)
		{
			break;
		}
	}
}

void UCameraSystemComponent::ResolveCameraStateParams()
{
	
}

void UCameraSystemComponent::AddCameraParameters(UCameraParamsDataAsset* CameraParams)
{
	CameraParamsArray.AddUnique(CameraParams);
}

void UCameraSystemComponent::RemoveCameraParameters(UCameraParamsDataAsset* CameraParams)
{
	CameraParamsArray.Remove(CameraParams);
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
			InterpSpeed = 8.0f;
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