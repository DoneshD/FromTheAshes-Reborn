// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourInterface.h"
#include "Components/ActorComponent.h"
#include "ParkourSystemComponent.generated.h"


class UCapsuleComponent;
class UCharacterMovementComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UParkourSystemComponent : public UActorComponent, public IParkourInterface
{
	GENERATED_BODY()


protected:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|Character")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|CharacterMovement")
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|SkeletalMesh")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|AnimInstance")
	TObjectPtr<UAnimInstance> AnimInstance;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|CapsuleComponent")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|CameraBoomComponent")
	TObjectPtr<USpringArmComponent> CameraBoomComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|CameraComponent")
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Ref|MotionWarpingComponent")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AWidgetActor> WidgetActorClass;

	//UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class AWidgetActor> WidgetActor;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AArrowActor> ArrowActorClass;

	//UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class AArrowActor> ArrowActor;

	float ArrowLocationX;
	float CharacterHeightDiff;
	float ArrowLocationZ = 195;

	float FirstTargetArmLength;
	FVector FirstArmRelativeLocation;

	UPROPERTY()
	float CapsuleRadius;

	UPROPERTY()
	float CapsuleHalfHeight;
	
	UPROPERTY()
	float JumpZVelocity;

	

public:
	UParkourSystemComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ParkourInputPressedVault();

	virtual bool SetIntializeReference(ACharacter* Character, USpringArmComponent* CameraBoom, UCameraComponent* Camera,
		UMotionWarpingComponent* MotionWarping) override;


};