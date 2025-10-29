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
	
	FVector DefaultCameraAnchorRelativeLocation;
	FRotator DefaultCameraAnchorRelativeRotation;

	FVector DefaultSpringArmRelativeLocation;
	FRotator DefaultSpringArmRelativeRotation;
	float DefaultSpringArmLength;

	FVector DefaultCameraComponentRelativeLocation;
	FRotator DefaultCameraComponentRelativeRotation;

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

	FVector GetDefaultCameraAnchorRelativeLocation() const { return DefaultCameraAnchorRelativeLocation; }
	FRotator GetDefaultCameraAnchorRelativeRotation() const { return DefaultCameraAnchorRelativeRotation; }

	FVector GetDefaultSpringArmRelativeLocation() const { return DefaultSpringArmRelativeLocation; }
	FRotator GetDefaultSpringArmRelativeRotation() const { return DefaultSpringArmRelativeRotation; }
	float GetDefaultSpringArmLength() const { return DefaultSpringArmLength; }

	FVector GetDefaultCameraComponentRelativeLocation() const { return DefaultCameraComponentRelativeLocation; }
	FRotator GetDefaultCameraComponentRelativeRotation() const { return DefaultCameraComponentRelativeRotation; }

	USceneComponent* GetCameraAnchorComponent();
	USpringArmComponent* GetSpringArmComponent();
	UCameraComponent* GetCameraComponentComponent();
	
	UFUNCTION()
	void OnTargetLockedOn(AActor* Target);

	UFUNCTION()
	void OnTargetLockedOff(AActor* Target);

};