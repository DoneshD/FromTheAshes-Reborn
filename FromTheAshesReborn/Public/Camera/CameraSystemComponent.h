#pragma once

#include "CoreMinimal.h"
#include "CameraSystemParams.h"
#include "Components/ActorComponent.h"
#include "CameraSystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraSystemAdjusted, FCameraSystemParams, CameraParams);

class USpringArmComponent;
class UCameraComponent;
class APlayerCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UCameraSystemComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY()
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY()
	TObjectPtr<USceneComponent> CameraAnchorComponent;

	float DefaultSpringArmLength = 400.0f;
	float BaseArmLengthFromTargetingSystem = 400.0f;
	float ArmLengthOffset = 0.0f;
	float ArmLengthLerpSpeed = 0.0f;
	float NewSpringArmLength = 0.0f;
	
	
	float CameraBaseFOV = 90.0f;
	float CameraFOVOffset = 0.0f;
	float CameraFOVLerpSpeed = 2.0f;
	
	FVector DefaultCameraAnchorRelativeLocation;
	FRotator DefaultCameraAnchorRelativeRotation;

	FVector NewCameraAnchorLocation;
	FRotator NewCameraAnchorRotation;
	
	float CameraAnchorInterpSpeed = 5.0f;

	FVector AnchorTransformLocation;
	FRotator AnchorTransformRotation;

	bool UseWorldTransform = false;

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Camera")
	FOnCameraSystemAdjusted OnCameraSystemAdjusted;

public:
	
	UCameraSystemComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void HandleCameraSystemAdjustment(FCameraSystemParams Params);
	
	UFUNCTION()
	void HandleSpringArmAdjustment(float InDeltaLength, float InInterpSpeed, bool InShouldOverride, bool InShouldResetOffset);

	UFUNCTION()
	void HandleCameraComponentAdjustment(float InDeltaFOV, float InInterpSpeed, bool InShouldOverride, bool InShouldResetOffset);

	UFUNCTION()
	void HandleCameraAnchorAdjustment(FVector InLocation, FRotator InRotation, bool InTransformType, bool InShouldOverride, bool InShouldResetOffset, float
	                                  InInterpSpeed);
	
};
