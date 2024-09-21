#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "FTACustomBase/FTAEnums.h"
#include "GameFramework/PlayerController.h"
#include "FTAPlayerController.generated.h"

class UEnhancedInputComponent;
class UInputMappingContext;

UCLASS()
class FROMTHEASHESREBORN_API AFTAPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	//-------------------------TESTING INPUT QUEUE------------------------------//

	UPROPERTY(EditAnywhere)
	EAllowedInputs QueuedInput;

	UPROPERTY(EditAnywhere)
	bool IsInInputQueueWindow = false;
	
	UPROPERTY(EditAnywhere)
	TArray<EAllowedInputs> CurrentAllowedInputs;

	UPROPERTY(EditAnywhere)
	bool InputQueueEnabled = true;
	
	UFUNCTION(BlueprintCallable)
	void InputQueueUpdateAllowedInputsBegin(TArray<EAllowedInputs> AllowedInputs);

	UFUNCTION(BlueprintCallable)
	void InputQueueUpdateAllowedInputsEnd(TArray<EAllowedInputs> AllowedInputs);

	void ActivateLastQueuedInput();

	void AddInputToQueue(EAllowedInputs InputToQueue);
	
	//--------------------------------------------------------------------------//

	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultInputMappingContext;

	TObjectPtr<ACharacter> PlayerCharacter;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_LookMouse;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_Dash;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_LightAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_HeavyAttack;
	
	//For debugging
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_SlowTime;

	bool IsTimeSlowed = false;
	
public:

	AFTAPlayerController();

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	void SendLocalInputToASC(bool, EAbilityInputID);

	void HandleMoveActionPressed(const FInputActionValue& InputActionValue);
	void HandleMoveActionReleased(const FInputActionValue& InputActionValue);
	
	void HandleInputLookMouse(const FInputActionValue& InputActionValue);

	void HandleJumpActionPressed(const FInputActionValue& InputActionValue);
	void HandleJumpActionReleased(const FInputActionValue& InputActionValue);

	void HandleDashActionPressed(const FInputActionValue& InputActionValue);
	void HandleDashActionReleased(const FInputActionValue& InputActionValue);
	
	void HandleLightAttackActionPressed(const FInputActionValue& InputActionValue);
	void HandleLightAttackActionReleased(const FInputActionValue& InputActionValue);

	void HandleHeavyAttackActionPressed(const FInputActionValue& InputActionValue);
	void HandleHeavyAttackActionReleased(const FInputActionValue& InputActionValue);
	
	void InputSlowTime(const FInputActionValue& InputActionValue);
};
