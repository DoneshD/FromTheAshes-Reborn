#pragma once

#include "CoreMinimal.h"
#include "FTACustomBase/FTACharacter.h"
#include "PlayerCharacter.generated.h"

class UTargetingSystemComponent;
class UParkourSystemComponent;
class UTargetSystemComponent;

UCLASS()
class FROMTHEASHESREBORN_API APlayerCharacter : public AFTACharacter
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<UStaticMeshComponent> SpringArmAttachmentMesh;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* SpringArmComp;

protected:
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Targeting")
	TObjectPtr<UTargetingSystemComponent> TargetingSystemComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "ParkourSystem")
	TObjectPtr<UParkourSystemComponent> ParkourSystemComponent;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseLookUpRate;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	class UCameraComponent* CameraComp;


	//Might need to move to FTA parent, check ABP base Cast
	UPROPERTY(BlueprintReadWrite)
	bool DisableRootMotion = false;

	FVector InitialSpringMeshLocation;

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

};