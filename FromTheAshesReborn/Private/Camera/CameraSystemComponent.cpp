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

	if (SpringArmComponent)
	{
		HandleSpringArmParams(DeltaTime);
	}
	
	if (CameraComponent)
	{
		HandleCameraComponentParams(DeltaTime);
	}

	if (CameraAnchorComponent)
	{
		HandleCameraAnchorParams(DeltaTime);
	}
	
	if(OwnerPlayerController)
	{
		HandleControlRotationParams(DeltaTime);
	}
}

void UCameraSystemComponent::HandleSpringArmParams(float DeltaTime)
{
	float CurrentArmLength = SpringArmComponent->TargetArmLength;
	
	if (!FMath::IsNearlyEqual(CurrentArmLength, CurrentCameraStateParams->SpringArmParams.ArmLength.Value, 0.1f))
	{
		float InterpolatedArmLength = FMath::FInterpTo(
			CurrentArmLength,
			CurrentCameraStateParams->SpringArmParams.ArmLength.Value,
			DeltaTime,
			CurrentCameraStateParams->SpringArmParams.ArmLength.MetaData.InLerpSpeedFloat);
		
		SpringArmComponent->TargetArmLength = InterpolatedArmLength;
	}

	FVector CurrentSocketOffset = SpringArmComponent->SocketOffset;
	
	if (!CurrentSocketOffset.Equals(CurrentCameraStateParams->SpringArmParams.SocketOffset.Value, 0.1f))
	{
			
		FVector InterpolatedSocketOffset = FMath::VInterpTo(
				CurrentSocketOffset,
				CurrentCameraStateParams->SpringArmParams.SocketOffset.Value,
				GetWorld()->GetDeltaSeconds(),
				CurrentCameraStateParams->SpringArmParams.SocketOffset.MetaData.InLerpSpeedFloat);
			
		SpringArmComponent->SocketOffset = InterpolatedSocketOffset;
			
			
	}
}

void UCameraSystemComponent::HandleCameraComponentParams(float DeltaTime)
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

void UCameraSystemComponent::HandleCameraAnchorParams(float DeltaTime)
{
	if(CurrentCameraStateParams->CameraAnchorParams.UseWorldTransform)
		{
			FVector CurrentAnchorLocation = CameraAnchorComponent->GetComponentLocation();
	
			if (!CurrentAnchorLocation.Equals(CurrentCameraStateParams->CameraAnchorParams.TargetLocation.Value, 0.1f))
			{
				// FVector InterpolatedLocation = FMath::VInterpTo(CurrentAnchorLocation, NewCameraAnchorLocation, GetWorld()->GetDeltaSeconds(), CameraAnchorInterpSpeed);
				FVector InterpolatedAnchorLocation = FMath::Lerp(CurrentAnchorLocation, CurrentCameraStateParams->CameraAnchorParams.TargetLocation.Value, 1.0f);
				CameraAnchorComponent->SetWorldLocation(InterpolatedAnchorLocation);

			}
	
			FRotator CurrentAnchorRotation = CameraAnchorComponent->GetComponentRotation();
	
			if (!CurrentAnchorRotation.Equals(CurrentCameraStateParams->CameraAnchorParams.TargetRotation.Value, 0.1f))
			{
				FRotator InterpolatedAnchorRotation = FMath::RInterpTo(
					CurrentAnchorRotation,
					CurrentCameraStateParams->CameraAnchorParams.TargetRotation.Value,
					DeltaTime,
					CurrentCameraStateParams->CameraAnchorParams.TargetRotation.MetaData.InLerpSpeedFloat);
				CameraAnchorComponent->SetWorldRotation(InterpolatedAnchorRotation);
			}
		}
	
		if(!CurrentCameraStateParams->CameraAnchorParams.UseWorldTransform)
		{
			FVector CurrentAnchorLocation = CameraAnchorComponent->GetRelativeLocation();
	
			if (!CurrentAnchorLocation.Equals(CurrentCameraStateParams->CameraAnchorParams.TargetLocation.Value, 0.1f))
			{
				FVector InterpolatedLocation = FMath::VInterpTo(
					CurrentAnchorLocation,
					CurrentCameraStateParams->CameraAnchorParams.TargetLocation.Value,
					DeltaTime,
					CurrentCameraStateParams->CameraAnchorParams.TargetLocation.MetaData.InLerpSpeedFloat);
				
				CameraAnchorComponent->SetRelativeLocation(InterpolatedLocation);
			}
	
			FRotator CurrentAnchorRotation = CameraAnchorComponent->GetRelativeRotation();
	
			if (!CurrentAnchorRotation.Equals(CurrentCameraStateParams->CameraAnchorParams.TargetRotation.Value, 0.1f))
			{
				FRotator InterpolatedRotation = FMath::RInterpTo(
					CurrentAnchorRotation,
					CurrentCameraStateParams->CameraAnchorParams.TargetRotation.Value,
					DeltaTime,
					CurrentCameraStateParams->CameraAnchorParams.TargetRotation.MetaData.InLerpSpeedFloat);
				CameraAnchorComponent->SetRelativeRotation(InterpolatedRotation);
			}
		}
	}

void UCameraSystemComponent::HandleControlRotationParams(float DeltaTime)
{
	FRotator FinalRotation = FMath::RInterpTo(
			OwnerPlayerController->GetControlRotation(),
			CurrentCameraStateParams->ControlRotationParams.TargetControlRotation.Value,
			DeltaTime,
			CurrentCameraStateParams->ControlRotationParams.TargetControlRotation.MetaData.InLerpSpeedFloat);
		
	if(CurrentCameraStateParams->ControlRotationParams.UseControllerRotation)
	{
		FinalRotation = OwnerPlayerController->GetControlRotation();
	}
	OwnerPlayerController->SetControlRotation(FinalRotation);
}

void UCameraSystemComponent::ResolveSpringArmParams()
{
    if (!CurrentCameraStateParams) return;

	ResolveSpringArmLength();
	ResolveSpringArmSocketOffset();
	
}

void UCameraSystemComponent::ResolveSpringArmLength()
{
	//Arm length
	ResolveCameraParam<float, FCameraFloatParam>(
		CameraParamsArray,
		CurrentCameraStateParams->SpringArmParams.ArmLength.Value,
		CurrentCameraStateParams->SpringArmParams.ArmLength.MetaData.InLerpSpeedFloat,
		CurrentCameraStateParams->SpringArmParams.ArmLength.MetaData.Priority,
		FCameraParamAccessors<float, FCameraFloatParam>
		{
			[](const UCameraParamsDataAsset* CameraParamAsset) -> const FCameraFloatParam& { return CameraParamAsset->SpringArmParams.ArmLength; },
			[](const FCameraFloatParam& FloatParam) -> const FCameraValueData& { return FloatParam.MetaData; },
			[](const FCameraFloatParam& FloatParam) -> const float& { return FloatParam.Value; },
			[](float& Target, const float& Value) { Target = Value; },
			[](const float& Target, const float& Value) { return Target + Value; },
			[](const FCameraValueData& MetaData) -> float {return MetaData.InLerpSpeedFloat;},
			[](const FCameraValueData& MetaData) -> float {return MetaData.Priority;}
			
        	
		}
	);
}

void UCameraSystemComponent::ResolveSpringArmSocketOffset()
{
	//Socket Offset
	ResolveCameraParam<FVector, FCameraVectorParam>(
		CameraParamsArray,
		CurrentCameraStateParams->SpringArmParams.SocketOffset.Value,
		CurrentCameraStateParams->SpringArmParams.SocketOffset.MetaData.InLerpSpeedFloat,
		CurrentCameraStateParams->SpringArmParams.SocketOffset.MetaData.Priority,
		FCameraParamAccessors<FVector, FCameraVectorParam>
		{
			[](const UCameraParamsDataAsset* CameraParamsAsset) -> const FCameraVectorParam& {return CameraParamsAsset->SpringArmParams.SocketOffset; },
			[](const FCameraVectorParam& VectorParam) -> const FCameraValueData& { return VectorParam.MetaData; },
			[](const FCameraVectorParam& VectorParam) -> const FVector& { return VectorParam.Value; },
			[](FVector& Target, const FVector& Value){ Target = Value; },
			[](const FVector& Target, const FVector& Value){ return Target + Value; },
			[](const FCameraValueData& MetaData) -> float {return MetaData.InLerpSpeedFloat;},
			[](const FCameraValueData& MetaData) -> float {return MetaData.Priority;}
		}

	);
}

void UCameraSystemComponent::ResolveControlRotationParams()
{
	ResolveTargetControlRotation();
}

void UCameraSystemComponent::ResolveTargetControlRotation()
{
	ResolveCameraParam<FRotator, FCameraRotatorParam>(
		CameraParamsArray,
		CurrentCameraStateParams->ControlRotationParams.TargetControlRotation.Value,
		CurrentCameraStateParams->ControlRotationParams.TargetControlRotation.MetaData.InLerpSpeedFloat,
		CurrentCameraStateParams->ControlRotationParams.TargetControlRotation.MetaData.Priority,
		FCameraParamAccessors<FRotator, FCameraRotatorParam>
		{
			[](const UCameraParamsDataAsset* CameraParamsAsset) -> const FCameraRotatorParam& {return CameraParamsAsset->ControlRotationParams.TargetControlRotation; },
			[](const FCameraRotatorParam& RotatorParam) -> const FCameraValueData& { return RotatorParam.MetaData; },
			[](const FCameraRotatorParam& RotatorParam) -> const FRotator& { return RotatorParam.Value; },
			[](FRotator& Target, const FRotator& Value){ Target = Value; },
			[this](const FRotator& Target, const FRotator& Value)
						{
							FRotator Result = OwnerPlayerController->GetControlRotation() + Value;
							return Result;
						},
			[](const FCameraValueData& MetaData) -> float
			{
				return MetaData.InLerpSpeedFloat;
			},
			[](const FCameraValueData& MetaData) -> float
			{
				return MetaData.Priority;
			}
		},

		[this](FRotator& OutValue, const FCameraRotatorParam& Param, const UCameraParamsDataAsset* CameraParamsAsset)
		{
			if (!CameraParamsAsset)
			{
				return;
			}
			CurrentCameraStateParams->ControlRotationParams.UseControllerRotation = CameraParamsAsset->ControlRotationParams.UseControllerRotation;
			
		}
	);
}

void UCameraSystemComponent::ResolveCameraAnchorParams()
{
	ResolveCameraAnchorLocation();
}

void UCameraSystemComponent::ResolveCameraAnchorLocation()
{
	ResolveCameraParam<FVector, FCameraVectorParam>(
		CameraParamsArray,
		CurrentCameraStateParams->CameraAnchorParams.TargetLocation.Value,
		CurrentCameraStateParams->CameraAnchorParams.TargetLocation.MetaData.InLerpSpeedFloat,
		CurrentCameraStateParams->CameraAnchorParams.TargetLocation.MetaData.Priority,
		FCameraParamAccessors<FVector, FCameraVectorParam>
		{
			[](const UCameraParamsDataAsset* CameraParamsAsset) -> const FCameraVectorParam& {return CameraParamsAsset->CameraAnchorParams.TargetLocation; },
			[](const FCameraVectorParam& VectorParam) -> const FCameraValueData& { return VectorParam.MetaData; },
			[](const FCameraVectorParam& VectorParam) -> const FVector& { return VectorParam.Value; },
			[](FVector& Target, const FVector& Value){ Target = Value; },
			[](const FVector& Target, const FVector& Value){ return Target + Value; },
			[](const FCameraValueData& MetaData) -> float {return MetaData.InLerpSpeedFloat;},
			[](const FCameraValueData& MetaData) -> float {return MetaData.Priority;}
		},

		[this](FVector& OutValue, const FCameraVectorParam& Param, const UCameraParamsDataAsset* CameraParamsAsset)
		{
			if (!CameraParamsAsset)
			{
				return;
			}
			if(!CameraParamsAsset->CameraAnchorParams.TargetLocation.MetaData.ShouldAdjust)
			{
				return;
			}
			CurrentCameraStateParams->CameraAnchorParams.UseWorldTransform = CameraParamsAsset->CameraAnchorParams.UseWorldTransform;
			
		}

	);
}

void UCameraSystemComponent::ResolveCameraParams()
{
	ResolveSpringArmParams();
	ResolveCameraAnchorParams();
	
	ResolveControlRotationParams();
	
	
}

void UCameraSystemComponent::AddCameraParameters(UCameraParamsDataAsset* CameraParams)
{
	CameraParamsArray.AddUnique(CameraParams);
	ResolveCameraParams();
}

void UCameraSystemComponent::RemoveCameraParameters(UCameraParamsDataAsset* CameraParams)
{
	CameraParamsArray.Remove(CameraParams);
	ResolveCameraParams();
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