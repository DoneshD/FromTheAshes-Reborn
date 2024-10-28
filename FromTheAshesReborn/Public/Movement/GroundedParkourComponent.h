﻿#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkourSystem/ParkourInterface.h"
#include "GroundedParkourComponent.generated.h"


class UCharacterMovementComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UGroundedParkourComponent : public UActorComponent, public IParkourInterface
{
	GENERATED_BODY()


protected:

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY()
	float CapsuleRadius;

	UPROPERTY()
	float CapsuleHalfHeight;
	
	UPROPERTY()
	float JumpZVelocity;

public:
	UGroundedParkourComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ParkourInputPressedVault();

	virtual bool SetIntializeReference(ACharacter* Character, USpringArmComponent* CameraBoom, UCameraComponent* Camera,
		UMotionWarpingComponent* MotionWarping) override;


};