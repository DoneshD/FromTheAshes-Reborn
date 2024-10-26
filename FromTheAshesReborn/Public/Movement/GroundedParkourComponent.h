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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grounded Parkour System|Vault")
	float HorizontalTraceDistanceVault = 220.0f;

public:
	UGroundedParkourComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void ParkourInputPressedVault();

	bool ParkourMasterTracerVault();

	bool GetForwardDistanceTrace(FHitResult& OutHitResult);
};