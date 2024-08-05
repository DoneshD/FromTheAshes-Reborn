#include "Player/FTAPlayerController.h"
#include "Player/FTAPlayerState.h"
#include "Player/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

AFTAPlayerController::AFTAPlayerController()
{
	
}

void AFTAPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PlayerCharacter = Cast<APlayerCharacter>(InPawn);
	if(!PlayerCharacter) { return; }

	AFTAPlayerState* PS = GetPlayerState<AFTAPlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
	
	EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if(!EnhancedInputComponent) { return; }
	
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(!InputSubsystem) { return; }
	check(InputSubsystem);

	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(DefaultInputMappingContext, 0);
	
	EnhancedInputComponent->BindAction(Input_Move, ETriggerEvent::Triggered, this, &AFTAPlayerController::HandleMoveActionPressed);
	EnhancedInputComponent->BindAction(Input_Move, ETriggerEvent::Completed, this, &AFTAPlayerController::HandleMoveActionReleased);
	
	EnhancedInputComponent->BindAction(Input_LookMouse, ETriggerEvent::Triggered, this, &AFTAPlayerController::HandleInputLookMouse);
	
	EnhancedInputComponent->BindAction(Input_LightAttack, ETriggerEvent::Started, this, &AFTAPlayerController::HandleLightAttackActionPressed);
	EnhancedInputComponent->BindAction(Input_LightAttack, ETriggerEvent::Completed, this, &AFTAPlayerController::HandleLightAttackActionReleased);

	//Debug purposes
	EnhancedInputComponent->BindAction(Input_SlowTime, ETriggerEvent::Started, this, &AFTAPlayerController::InputSlowTime);

}

void AFTAPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	EnhancedInputComponent->ClearActionBindings();
}

void AFTAPlayerController::SendLocalInputToASC(bool IsPressed, EAbilityInputID AbilityInputID)
{
	AFTAPlayerState* FTAPlayerState = GetPlayerState<AFTAPlayerState>();
	
	if(!FTAPlayerState) { return; }
	
	UFTAAbilitySystemComponent* AbilitySystemComponent = CastChecked<UFTAAbilitySystemComponent>(FTAPlayerState->GetAbilitySystemComponent());
	if(!AbilitySystemComponent) { return; }

	if(IsPressed)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(AbilityInputID));
	}
	else
	{
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(AbilityInputID));
	}
}

void AFTAPlayerController::HandleMoveActionPressed(const FInputActionValue& InputActionValue)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	FVector2d InputDirection = InputActionValue.Get<FVector2d>();
	InputDirection.Normalize();

	InputDirection.X = FMath::Clamp(InputDirection.X, -1.0f, 1.0f);
	InputDirection.Y = FMath::Clamp(InputDirection.Y, -1.0f, 1.0f);

	if(!PlayerCharacter) { return; }

	PlayerCharacter->AddMovementInput(ControlRot.Vector(), InputDirection.Y);

	const FRotationMatrix RotationMatrix(ControlRot);
	const FVector RightVector = RotationMatrix.GetScaledAxis(EAxis::Y);
	
	PlayerCharacter->AddMovementInput(RightVector, InputDirection.X);

	SendLocalInputToASC(true, EAbilityInputID::Move);
}

void AFTAPlayerController::HandleMoveActionReleased(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(false, EAbilityInputID::Move);
}

void AFTAPlayerController::HandleInputLookMouse(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void AFTAPlayerController::HandleLightAttackActionPressed(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(true, EAbilityInputID::LightAttack);
	
}

void AFTAPlayerController::HandleLightAttackActionReleased(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(false, EAbilityInputID::LightAttack);
}

void AFTAPlayerController::InputSlowTime(const FInputActionValue& InputActionValue)
{
	
}
