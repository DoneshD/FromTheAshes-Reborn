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
		if (!FMath::IsNearlyEqual(CurrentLength, SpringArmTargetLength, 0.1f))
		{
			SpringArmComponent->TargetArmLength = FMath::FInterpTo(CurrentLength, SpringArmTargetLength, DeltaTime, SpringArmLerpSpeed);
		}
	}

}

void UCameraSystemComponent::HandleSpringArmAdjustment(float InDeltaLength, float InInterpSpeed)
{
	if (SpringArmComponent)
	{
		SpringArmTargetLength = SpringArmComponent->TargetArmLength + InDeltaLength;
		SpringArmLerpSpeed = InInterpSpeed;
	}
}

void UCameraSystemComponent::HandleCameraSystemAdjustment(FCameraSystemParams Params)
{
	if(Params.ShouldAdjustArmLength)
	{
		HandleSpringArmAdjustment(Params.DeltaArmLength, Params.DeltaArmLengthInterpSpeed);
	}
}

