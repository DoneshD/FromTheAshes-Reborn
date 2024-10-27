#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GroundedParkourComponent.generated.h"


class UCharacterMovementComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UGroundedParkourComponent : public UActorComponent
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

	UPROPERTY()
	bool CanParkour;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded Parkour System|Vault")
	float TraceForwardActorDistance = 300.0f;

	UPROPERTY()
	TObjectPtr<AActor> TraceFirstActor;

	UPROPERTY()
	FVector TraceFirstActorImpactPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded Parkour System|Vault")
	float CheckVerticalBlockerDistance = 150.0f;

public:
	UGroundedParkourComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ParkourInputPressedVault();

	bool ParkourMasterTracerVault();

	bool GetForwardActorTrace();

	bool GetVerticalBlockerTrace();

};