#pragma once

#include "CoreMinimal.h"
#include "FTACustomBase/FTACharacter.h"
#include "Interfaces/MeleeCombatantInterface.h"
#include "PlayerCharacter.generated.h"

class UPlayerComboManagerComponent;
class UGroundedMeleeComboComponent;
class UGameplayEffect;
class UTargetSystemComponent;

UCLASS()
class FROMTHEASHESREBORN_API APlayerCharacter : public AFTACharacter, public IMeleeCombatantInterface
{
	GENERATED_BODY()
	
public:
	
	APlayerCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	void InitAbilitySystemComponent();
	
	virtual void FinishDying() override;

	//Move to super class
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mesh")
	USkeletalMeshComponent* GetSkeletalMesh() const;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGroundedMeleeComboComponent> GroundedMeleeComboComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerComboManagerComponent> PlayerComboManagerComponent;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseLookUpRate;
	
	bool IsASCInputBound;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	class UCameraComponent* CameraComp;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Targeting")
	TObjectPtr<UTargetSystemComponent> TargetSystemComponent;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GameplayEffect | Death")
	TSubclassOf<UGameplayEffect> DeathEffect;
	
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PostInitializeComponents() override;
	
	//---------------------------------------INTERFACES-----------------------------------//

	virtual FGameplayTagContainer& GetCurrentComboContainer() override;

	virtual int32 GetCurrentComboIndex() override;

	virtual void SetCurrentComboIndex(int ComboIndex) override;

	virtual void RegisterGameplayTagEvent(FGameplayTag InputSavedTag, FGameplayTag ComboWindow, FTimerHandle FComboWindowTimer) override;
};