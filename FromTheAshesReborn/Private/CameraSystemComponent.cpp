#include "CameraSystemComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HelperFunctionLibraries/ViewportUtilityFunctionLibrary.h"

UCameraSystemComponent::UCameraSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCameraSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());

	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCharacter is null"));
		return;
	}

	CameraAnchorComponent = PlayerCharacter->GetCameraAnchorComponent();

	if (!IsValid(CameraAnchorComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("CameraAnchorComponent is null"));
		return;
	}

	CameraComponent = PlayerCharacter->GetCameraComponentComponent();

	if (!CameraComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CameraComponent is null"));
		return;
	}
	
	SpringArmComponent = PlayerCharacter->GetSpringArmComponent();
	
	if (!SpringArmComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("SpringArmComponent is null"));
		return;
	}

	BaseSpringArmLength = PlayerCharacter->GetDefaultSpringArmLength();
	BaseAnchorLocation = PlayerCharacter->GetDefaultCameraAnchorLocation();
	BaseAnchorRotation = PlayerCharacter->GetDefaultCameraAnchorRotation();

	OnCameraSystemAdjusted.AddDynamic(this, &UCameraSystemComponent::HandleCameraSystemAdjustment);
}

void UCameraSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SpringArmComponent)
	{
		float CurrentLength = SpringArmComponent->TargetArmLength;
		float FinalTargetLength = BaseArmLengthFromTargetingSystem + ArmLengthOffset;

		if (!FMath::IsNearlyEqual(CurrentLength, FinalTargetLength, 0.1f))
		{
			float InterpolatedLength = FMath::FInterpTo(CurrentLength, FinalTargetLength, DeltaTime, ArmLengthLerpSpeed);
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
		FVector CurrentAnchorLocation = PlayerCharacter->CameraAnchorComponent->GetRelativeLocation();

		if (!CurrentAnchorLocation.Equals(BaseAnchorLocation, 0.1f))
		{
			FVector InterpolatedLocation = FMath::VInterpTo(CurrentAnchorLocation, BaseAnchorLocation, GetWorld()->GetDeltaSeconds(), CameraAnchorInterpSpeed);
			CameraAnchorComponent->SetWorldLocation(InterpolatedLocation);
		}
	}


}

void UCameraSystemComponent::HandleSpringArmAdjustment(float InDeltaLength, float InInterpSpeed, bool InShouldOverride, bool InShouldResetOffset)
{
	if (SpringArmComponent)
	{
		if (InShouldOverride)
		{
			BaseArmLengthFromTargetingSystem = InDeltaLength;

			if (InShouldResetOffset)
			{
				ArmLengthOffset = 0.0f;
			}
		}
		else
		{
			ArmLengthOffset += InDeltaLength;
		}

		ArmLengthLerpSpeed = InInterpSpeed;
	}
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

void UCameraSystemComponent::HandleCameraAnchorAdjustment(FVector InLocation, FRotator InRotation, bool InShouldOverride, bool InShouldResetOffset, float InInterpSpeed)
{
	if (CameraAnchorComponent)
	{
		if (InShouldOverride)
		{
			BaseAnchorLocation = InLocation;
			BaseAnchorRotation = InRotation;
		}
		CameraAnchorInterpSpeed = InInterpSpeed;
	}
}

void UCameraSystemComponent::HandleCameraSystemAdjustment(FCameraSystemParams Params)
{
	if(Params.ArmLengthParams.ShouldAdjustArmLength)
	{
		HandleSpringArmAdjustment(Params.ArmLengthParams.DeltaArmLength,
			Params.ArmLengthParams.DeltaArmLengthInterpSpeed,
			Params.ArmLengthParams.ShouldOverrideArmLength,
			Params.ArmLengthParams.ShouldResetOffset);
	}

	if(Params.CameraComponentParams.ShouldAdjustFOV)
	{
		HandleCameraComponentAdjustment(Params.CameraComponentParams.DeltaFOV,
			Params.CameraComponentParams.DeltaFOVInterpSpeed,
			Params.CameraComponentParams.ShouldOverrideFOV,
			Params.CameraComponentParams.ShouldResetFOVOffset);
	}

	// if(Params.CameraAnchorParams.ShouldAdjustAnchor)
	// {
	// 	HandleCameraAnchorAdjustment(Params.CameraAnchorParams.NewAnchorLocation,
	// 		Params.CameraAnchorParams.NewAnchorRotation,
	// 		Params.CameraAnchorParams.ShouldOverrideAnchor,
	// 		Params.CameraAnchorParams.ShouldResetAnchorOffset,
	// 		Params.CameraAnchorParams.DeltaAnchorInterpSpeed);
	// }
}

