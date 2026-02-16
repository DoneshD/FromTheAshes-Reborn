#pragma once

#include "CoreMinimal.h"
#include "CameraSystemParams.h"
#include "Components/ActorComponent.h"
#include "CameraSystemComponent.generated.h"

class UCameraParamsDataAsset;
class UTargetingSystemComponent;
class AFTAPlayerCameraManger;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraSystemAdjusted, FCameraSystemParams, CameraParams);

class USpringArmComponent;
class UCameraComponent;
class APlayerCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UCameraSystemComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	AActor* OwnerActor;

	UPROPERTY()
	APawn* OwnerPawn;

	UPROPERTY()
	TObjectPtr<AFTAPlayerCameraManger> FTAPlayerCameraManger;


protected:

	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY()
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY()
	TObjectPtr<USceneComponent> CameraAnchorComponent;

	UPROPERTY()
	TObjectPtr<UTargetingSystemComponent> TargetingSystemComponent;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float DefaultSpringArmLength = 250.0f;
	
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

	//Move from targeting

	FRotator CurrentCameraOffset;

	FVector SmoothedMidPoint = FVector::ZeroVector;



public:
	
	UPROPERTY(BlueprintAssignable, Category = "Camera")
	FOnCameraSystemAdjusted OnCameraSystemAdjusted;

	UPROPERTY()
	APlayerController* OwnerPlayerController;

public:
	
	UCameraSystemComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION()
	void HandleCameraSystemAdjustment(UCameraParamsDataAsset* Params);
	
	UFUNCTION()
	void HandleSpringArmAdjustment(float InDeltaLength, float InInterpSpeed, bool InShouldOverride, bool InShouldResetOffset);

	UFUNCTION()
	void HandleCameraComponentAdjustment(float InDeltaFOV, float InInterpSpeed, bool InShouldOverride, bool InShouldResetOffset);

	UFUNCTION()
	void HandleCameraAnchorAdjustment(FVector InLocation, FRotator InRotation, bool InTransformType, bool InShouldOverride, bool InShouldResetOffset, float
	                                  InInterpSpeed);


	//Moving from targeting system
	void NeutralCameraState();
	void ControlCameraOffset(float DeltaTime, TObjectPtr<UCameraParamsDataAsset> CameraParams);
	void SetupLocalPlayerController();
	void DrawCameraAnchor();
	void UpdateTargetingCameraAnchorAndRotation(APlayerCharacter* PlayerOwner, const AActor* TargetActor, float DeltaTime, TObjectPtr<UCameraParamsDataAsset> CameraParams);
	float CatchupToOffScreen(const FVector& PlayerLocation, float& InInterpSpeed, TObjectPtr<UCameraParamsDataAsset> CameraParams);
	float CompareDistanceToScreenAndGetInterpSpeed(APlayerCharacter* PlayerOwner, const AActor* TargetActor, bool& InShouldUpdateControlRotation);
	float GetWorldDistanceFromCamera(APlayerController* PlayerController, const AActor* TargetActor);
	bool ShouldUpdateControllerRotation = false;
	FRotator AddDistanceBasedAndInputOffset(const AActor* OtherActor, TObjectPtr<UCameraParamsDataAsset> CameraParams) const;
	static FRotator FindLookAtRotation(const FVector Start, const FVector Target);
	float GetDistanceFromCharacter(const AActor* OtherActor) const;
	float CalculateControlRotationOffset(float Distance, float MaxOffset) const;
};
