#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraSystemComponent.generated.h"

class UCameraParamsDataAsset;
class UTargetingSystemComponent;
class AFTAPlayerCameraManger;

class USpringArmComponent;
class UCameraComponent;
class APlayerCharacter;

UCLASS(BlueprintType, Const)
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

	UPROPERTY()
	TArray<TObjectPtr<UCameraParamsDataAsset>> CameraParamsArray;
	
	float LockOnLerpSpeed;

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
	
	float TargetSpringArmLength = 0.0f;
	float ArmLengthLerpSpeed = 2.0f;
	
	float CameraBaseFOV = 90.0f;
	float CameraFOVOffset = 0.0f;
	float CameraFOVLerpSpeed = 2.0f;

	FVector TargetCameraAnchorLocation;
	FRotator TargetCameraAnchorRotation;
	FRotator TargetControlRotation;
	
	float CameraAnchorInterpSpeed = 2.0f;

	//Move from targeting

	FRotator CurrentCameraOffset;

	FVector SmoothedMidPoint = FVector::ZeroVector;

	FVector LockOnTargetLocation = FVector::ZeroVector;
	FRotator LockOnTargetRotation = FRotator::ZeroRotator;
	
	bool ShouldUpdateControllerRotation = false;


public:

	bool UseWorldTransform = false;
	float LockOnSpringArmLength = 0.0f;
	
	UPROPERTY()
	APlayerController* OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UCameraParamsDataAsset> NeutralCameraStateParams;

public:
	
	UCameraSystemComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetupLocalPlayerController();
	
	float ResolveSpringArmLength();
	void ResolveCameraAnchorTransform();
	void ResolveControlRotation();
	
	UFUNCTION(BlueprintCallable)
	void AddCameraParameters(UCameraParamsDataAsset* CameraParams);

	UFUNCTION(BlueprintCallable)
	void RemoveCameraParameters(UCameraParamsDataAsset* CameraParams);
	
	float CatchupToOffScreen(const FVector& PlayerLocation, float& InInterpSpeed, TObjectPtr<UCameraParamsDataAsset> CameraParams);
	float CompareDistanceToScreenAndGetInterpSpeed(APlayerCharacter* PlayerOwner, const AActor* TargetActor, bool& InShouldUpdateControlRotation);
	float GetWorldDistanceFromCamera(APlayerController* PlayerController, const AActor* TargetActor);
	
};
