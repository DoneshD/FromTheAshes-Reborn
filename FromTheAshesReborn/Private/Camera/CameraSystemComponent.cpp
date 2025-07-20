#include "Camera/CameraSystemComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HelperFunctionLibraries/ViewportUtilityFunctionLibrary.h"

UCameraSystemComponent::UCameraSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	DefaultSpringArmLength = 400.0f;
	BaseArmLengthFromTargetingSystem = 400.0f;
	ArmLengthOffset = 0.0f;
	ArmLengthLerpSpeed = 0.0f;
	NewSpringArmLength = 0.0f;

	CameraBaseFOV = 90.0f;
	CameraFOVOffset = 0.0f;
	CameraFOVLerpSpeed = 2.0f;

	CameraAnchorInterpSpeed = 5.0f;

	UseWorldTransform = false;

	DefaultCameraAnchorRelativeLocation = FVector::ZeroVector;
	DefaultCameraAnchorRelativeRotation = FRotator::ZeroRotator;

	NewCameraAnchorLocation = FVector::ZeroVector;
	NewCameraAnchorRotation = FRotator::ZeroRotator;

	AnchorTransformLocation = FVector::ZeroVector;
	AnchorTransformRotation = FRotator::ZeroRotator;
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

	DefaultSpringArmLength = PlayerCharacter->GetDefaultSpringArmLength();
	DefaultCameraAnchorRelativeLocation = PlayerCharacter->GetDefaultCameraAnchorRelativeLocation();
	DefaultCameraAnchorRelativeRotation = PlayerCharacter->GetDefaultCameraAnchorRelativeRotation();

	// OnCameraSystemAdjusted.AddDynamic(this, &UCameraSystemComponent::HandleCameraSystemAdjustment);
}

void UCameraSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SpringArmComponent)
	{
		
			float CurrentLength = SpringArmComponent->TargetArmLength;
			float FinalTargetLength = NewSpringArmLength + ArmLengthOffset;
		
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

void UCameraSystemComponent::HandleSpringArmAdjustment(float InDeltaLength, float InInterpSpeed, bool InShouldOverride, bool InShouldResetOffset)
{
	if (SpringArmComponent)
	{
		if (InShouldOverride)
		{
			// BaseArmLengthFromTargetingSystem = InDeltaLength;
			NewSpringArmLength = InDeltaLength;

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

	if(Params.CameraAnchorParams.ShouldAdjustAnchor)
	{
		HandleCameraAnchorAdjustment(Params.CameraAnchorParams.NewAnchorLocation,
			Params.CameraAnchorParams.NewAnchorRotation,
			Params.CameraAnchorParams.ShouldUseWorldTransform, 
			Params.CameraAnchorParams.ShouldOverrideAnchor,
			Params.CameraAnchorParams.ShouldResetAnchorOffset,
			Params.CameraAnchorParams.DeltaAnchorInterpSpeed);
	}
}

