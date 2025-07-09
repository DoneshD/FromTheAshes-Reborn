#pragma once

#include "CoreMinimal.h"
#include "FTACustomBase/FTACharacter.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UCameraSystemComponent;
class UTargetingSystemComponent;
class UParkourSystemComponent;

UCLASS()
class FROMTHEASHESREBORN_API APlayerCharacter : public AFTACharacter
{
	GENERATED_BODY()

private:
	
	FVector DefaultCameraAnchorLocation;
	FRotator DefaultCameraAnchorRotation;

	FVector DefaultSpringArmLocation;
	FRotator DefaultSpringArmRotation;
	float DefaultSpringArmLength;

	FVector DefaultCameraComponentLocation;
	FRotator DefaultCameraComponentRotation;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USceneComponent> CameraAnchorComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

protected:
	
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	// TObjectPtr<UTargetingSystemComponent> TargetingSystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTargetingSystemComponent> TargetSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UParkourSystemComponent> ParkourSystemComponent;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	// TObjectPtr<UCameraSystemComponent> CameraSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraSystemComponent> CameraManagerComponent;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseLookUpRate;


public:
	
	UPROPERTY(BlueprintReadWrite)
	bool HasMovementInput = false;

public:

	APlayerCharacter(const class FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void InitAbilitySystemComponent() override;

	FVector GetDefaultCameraAnchorLocation() const { return DefaultCameraAnchorLocation; }
	FRotator GetDefaultCameraAnchorRotation() const { return DefaultCameraAnchorRotation; }

	FVector GetDefaultSpringArmLocation() const { return DefaultSpringArmLocation; }
	FRotator GetDefaultSpringArmRotation() const { return DefaultSpringArmRotation; }
	float GetDefaultSpringArmLength() const { return DefaultSpringArmLength; }

	FVector GetDefaultCameraComponentLocation() const { return DefaultCameraComponentLocation; }
	FRotator GetDefaultCameraComponentRotation() const { return DefaultCameraComponentRotation; }

	USceneComponent* GetCameraAnchorComponent();
	USpringArmComponent* GetSpringArmComponent();
	UCameraComponent* GetCameraComponentComponent();

};