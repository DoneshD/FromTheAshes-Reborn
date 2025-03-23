#pragma once

#include "CoreMinimal.h"
#include "FTACustomBase/FTACharacter.h"
#include "PlayerCharacter.generated.h"

class UMotionWarpingComponent;
class UParkourSystemComponent;
class UGroundedMeleeComboComponent;
class UGameplayEffect;
class UTargetSystemComponent;

UCLASS()
class FROMTHEASHESREBORN_API APlayerCharacter : public AFTACharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Targeting")
	TObjectPtr<UTargetSystemComponent> TargetSystemComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "ParkourSystem")
	TObjectPtr<UParkourSystemComponent> ParkourSystemComponent;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseLookUpRate;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	class UCameraComponent* CameraComp;

public:

	APlayerCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void InitAbilitySystemComponent() override;
	

};