#pragma once

#include "CoreMinimal.h"
#include "FTACustomBase/FTACharacter.h"
#include "Interfaces/PlayerComboManagerInterface.h"
#include "PlayerCharacter.generated.h"

class UMotionWarpingComponent;
class UParkourSystemComponent;
class UPlayerComboManagerComponent;
class UGroundedMeleeComboComponent;
class UGameplayEffect;
class UTargetSystemComponent;

UCLASS()
class FROMTHEASHESREBORN_API APlayerCharacter : public AFTACharacter, public IPlayerComboManagerInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "PlayerComboManager")
	TObjectPtr<UPlayerComboManagerComponent> PlayerComboManagerComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Targeting")
	TObjectPtr<UTargetSystemComponent> TargetSystemComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "ParkourSystem")
	TObjectPtr<UParkourSystemComponent> ParkourSystemComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "MotionWarping")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseLookUpRate;
	
	bool IsASCInputBound;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	class UCameraComponent* CameraComp;


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GameplayEffect | Death")
	TSubclassOf<UGameplayEffect> DeathEffect;

	APlayerCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	void InitAbilitySystemComponent();
	
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PostInitializeComponents() override;

	//Move to super class
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mesh")
	USkeletalMeshComponent* GetSkeletalMesh() const;
	
	//---------------------------------------INTERFACES-----------------------------------//

	virtual FGameplayTagContainer& GetCurrentComboContainer() override;

	virtual int32 GetCurrentComboIndex() override;

	virtual void SetCurrentComboIndex(int ComboIndex) override;

};