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

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USceneComponent> CameraAnchorComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComp;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTargetingSystemComponent> TargetingSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UParkourSystemComponent> ParkourSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraSystemComponent> CameraSystemComponent;
	
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

	FVector DefaultCameraAnchorRelativeLocation;
	FRotator DefaultCameraAnchorRelativeRotation;

};