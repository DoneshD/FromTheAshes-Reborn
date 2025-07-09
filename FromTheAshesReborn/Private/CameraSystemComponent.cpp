#include "CameraSystemComponent.h"

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

	OnCameraSystemAdjusted.AddDynamic(this, &UCameraSystemComponent::HandleCameraSystemAdjustment);
}

void UCameraSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SpringArmComponent)
	{
		const float CurrentLength = SpringArmComponent->TargetArmLength;
		const float FinalTargetLength = BaseArmLengthFromTargetingSystem + ArmLengthOffset;

		if (!FMath::IsNearlyEqual(CurrentLength, FinalTargetLength, 0.1f))
		{
			const float InterpolatedLength = FMath::FInterpTo(CurrentLength, FinalTargetLength, DeltaTime, SpringArmLerpSpeed);
			SpringArmComponent->TargetArmLength = InterpolatedLength;

			// UE_LOG(LogTemp, Warning, TEXT("Spring Arm Lerp Debug -> CurrentLength: %.2f, Base: %.2f, Offset: %.2f, FinalTarget: %.2f"),
			// 	CurrentLength, BaseArmLengthFromTargetingSystem, ArmLengthOffset, InterpolatedLength);
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

		SpringArmLerpSpeed = InInterpSpeed;
	}
}

void UCameraSystemComponent::HandleCameraSystemAdjustment(FCameraSystemParams Params)
{
	if(Params.ArmLengthParams.ShouldAdjustArmLength)
	{
		HandleSpringArmAdjustment(Params.ArmLengthParams.DeltaArmLength, Params.ArmLengthParams.DeltaArmLengthInterpSpeed, Params.ArmLengthParams.ShouldOverrideArmLength, Params.ArmLengthParams.ShouldResetOffset);
	}
}

