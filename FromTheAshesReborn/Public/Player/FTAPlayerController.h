#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "InputActionValue.h"
//damn...
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "FTACustomBase/FTAEnums.h"
#include "GameFramework/PlayerController.h"
#include "FTAPlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRegisterWindowTagEventSignature, FGameplayTag, ComboWindowTag);

class UFTAInputComponent;
class UPlayerComboManagerComponent;
class UAbilitySystemComponent;
class UEnhancedInputComponent;
class UInputMappingContext;
class UFTAInputConfig;

UCLASS()
class FROMTHEASHESREBORN_API AFTAPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FTA | Input")
	TObjectPtr<UInputMappingContext> DefaultInputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FTA | Input")
	TObjectPtr<UFTAInputConfig> FTAInputConfig;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<ACharacter> PlayerCharacter;

public:
	
	AFTAPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	UFUNCTION(BlueprintCallable, Category = "FTA|ASComponent")
	UFTAAbilitySystemComponent* GetFTAAbilitySystemComponent() const;
	
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);
	void InputAbilityInputTagPressed(FGameplayTag InputTag);
	void InputAbilityInputTagReleased(FGameplayTag InputTag);

	UPROPERTY(EditAnywhere)
	bool IsInInputQueueWindow = false;
	
protected:

	//-----------------------------For debugging--------------------------//
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_SlowTime;

	bool IsTimeSlowed = false;

	//-------------------------INPUT QUEUE------------------------------//
	
	
	TObjectPtr<UPlayerComboManagerComponent> PlayerComboManager;

	UFUNCTION(BlueprintCallable)
	void InputQueueAllowedInputsBegin(TArray<TSubclassOf<UFTAGameplayAbility>> QueueableAbilityClasses);

	UFUNCTION(BlueprintCallable)
	void InputQueueUpdateAllowedInputsEnd();
	
	void ProcessAbilityComboData(UFTAGameplayAbility* Ability);
	
	FRegisterWindowTagEventSignature OnRegisterWindowTagEventDelegate;


	void SendLocalInputToASC(bool, EAbilityInputID);

	void HandleMoveActionPressed(const FInputActionValue& InputActionValue);
	void HandleMoveActionReleased(const FInputActionValue& InputActionValue);
	
	void HandleInputLookMouse(const FInputActionValue& InputActionValue);
	
	void InputSlowTime(const FInputActionValue& InputActionValue);

};