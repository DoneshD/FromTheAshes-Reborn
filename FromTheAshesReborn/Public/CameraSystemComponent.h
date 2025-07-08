#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraSystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpringArmLengthAdjusted, float, DeltaLength);

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

	// UPROPERTY()
	// float BaseSpringArmLength = 400.0f;

	// UPROPERTY()
	// float CurrentSpringArmLength = 400.0f;

	UPROPERTY()
	float SpringArmLerpSpeed = 5.0f;

	UPROPERTY()
	float SpringArmTargetLength = 400.0f;

public:

	UPROPERTY(BlueprintAssignable, Category = "Camera")
	FOnSpringArmLengthAdjusted OnSpringArmLengthAdjusted;

public:
	
	UCameraSystemComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION()
	void HandleSpringArmAdjustment(float DeltaLength);
};
