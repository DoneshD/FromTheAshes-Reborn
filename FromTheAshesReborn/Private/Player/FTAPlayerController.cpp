#include "Player/FTAPlayerController.h"
#include "Player/FTAPlayerState.h"
#include "Player/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

void AFTAPlayerController::InputQueueUpdateAllowedInputsBegin(TArray<EAllowedInputs> AllowedInputs)
{
	CurrentAllowedInputs = AllowedInputs;
	IsInInputQueueWindow = true;

	GetWorld()->GetTimerManager().SetTimer(FInputQueueWindowTimer, this, &AFTAPlayerController::CheckLastQueuedInput, 0.01f, true);
}

void AFTAPlayerController::InputQueueUpdateAllowedInputsEnd(TArray<EAllowedInputs> AllowedInputs)
{
	IsInInputQueueWindow = false;
	QueuedInput = EAllowedInputs::None;
	CurrentAllowedInputs.Empty();
	GetWorld()->GetTimerManager().ClearTimer(FInputQueueWindowTimer);
}

void AFTAPlayerController::CheckLastQueuedInput()
{
	bool LastSavedInputTagTest = LastSavedInputTag.MatchesTag(FGameplayTag::RequestGameplayTag("Event.Input.Saved.Heavy"));
	bool ComboWindowTagTest = ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Event.Montage.ComboWindow.Open.Heavy"));
	switch (QueuedInput)
	{
	case EAllowedInputs::None:
		//UE_LOG(LogTemp, Warning, TEXT("No input queued."));
		break;
 
	case EAllowedInputs::Jump:
		//UE_LOG(LogTemp, Warning, TEXT("Jump input detected."));
		break;

	case EAllowedInputs::Dash:
		//UE_LOG(LogTemp, Warning, TEXT("Dash input detected."));
		break;

		
	case EAllowedInputs::LightAttack:

		if(LastSavedInputTag.MatchesTag(FGameplayTag::RequestGameplayTag("Event.Input.Saved.Light")) &&
			ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Event.Montage.ComboWindow.Open.Light")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Light attack Saved!!!!"));
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("Event.Input.Saved.Light"));
		}
		break;

	case EAllowedInputs::HeavyAttack:

		UE_LOG(LogTemp, Error, TEXT("LastSavedInputTagTest is: %s"), LastSavedInputTagTest ? TEXT("true") : TEXT("false"));
		UE_LOG(LogTemp, Error, TEXT("ComboWindowTagTest is: %s"), ComboWindowTagTest ? TEXT("true") : TEXT("false"));

		if(LastSavedInputTag.MatchesTag(FGameplayTag::RequestGameplayTag("Event.Input.Saved.Heavy")) &&
			ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Event.Montage.ComboWindow.Open.Heavy")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Heavy attack Saved!!!!"));
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("Event.Input.Saved.Heavy"));
		}
		break;

	default:
		//UE_LOG(LogTemp, Warning, TEXT("Unknown input."));
		break;
	}
}

void AFTAPlayerController::AddInputToQueue(EAllowedInputs InputToQueue, FGameplayTag SavedInputTag)
{
	if(IsInInputQueueWindow)
	{
		if(CurrentAllowedInputs.Num() == 0)
		{
			
		}
		else
		{
			if(CurrentAllowedInputs.Contains(InputToQueue))
			{
				LastSavedInputTag = SavedInputTag;
				QueuedInput = InputToQueue;
			}
		}
	}
}

AFTAPlayerController::AFTAPlayerController()
{
	
}

void AFTAPlayerController::BeginPlay()
{
	Super::BeginPlay();

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
		ASC = PS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(PS, InPawn);
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

	EnhancedInputComponent->BindAction(Input_HeavyAttack, ETriggerEvent::Started, this, &AFTAPlayerController::HandleHeavyAttackActionPressed);
	EnhancedInputComponent->BindAction(Input_HeavyAttack, ETriggerEvent::Completed, this, &AFTAPlayerController::HandleHeavyAttackActionReleased);

	EnhancedInputComponent->BindAction(Input_Jump, ETriggerEvent::Started, this, &AFTAPlayerController::HandleJumpActionPressed);
	EnhancedInputComponent->BindAction(Input_Jump, ETriggerEvent::Completed, this, &AFTAPlayerController::HandleJumpActionReleased);

	EnhancedInputComponent->BindAction(Input_Dash, ETriggerEvent::Started, this, &AFTAPlayerController::HandleDashActionPressed);
	EnhancedInputComponent->BindAction(Input_Dash, ETriggerEvent::Completed, this, &AFTAPlayerController::HandleDashActionReleased);

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

void AFTAPlayerController::HandleJumpActionPressed(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(true, EAbilityInputID::Jump);
}

void AFTAPlayerController::HandleJumpActionReleased(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(false, EAbilityInputID::Jump);
}

void AFTAPlayerController::HandleDashActionPressed(const FInputActionValue& InputActionValue)
{
	if(IsInInputQueueWindow)
	{
		AddInputToQueue(EAllowedInputs::Dash, FGameplayTag::RequestGameplayTag("Event.Input.Saved.Dash"));
	}
	else
	{
		SendLocalInputToASC(true, EAbilityInputID::Dash);

	}
}

void AFTAPlayerController::HandleDashActionReleased(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(false, EAbilityInputID::Dash);
}

void AFTAPlayerController::HandleLightAttackActionPressed(const FInputActionValue& InputActionValue)
{
	if(IsInInputQueueWindow)
	{
		AddInputToQueue(EAllowedInputs::LightAttack, FGameplayTag::RequestGameplayTag("Event.Input.Saved.Light"));
	}
	else
	{
		SendLocalInputToASC(true, EAbilityInputID::LightAttack);
	}
}

void AFTAPlayerController::HandleLightAttackActionReleased(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(false, EAbilityInputID::LightAttack);
}

void AFTAPlayerController::HandleHeavyAttackActionPressed(const FInputActionValue& InputActionValue)
{
	if(IsInInputQueueWindow)
	{
		AddInputToQueue(EAllowedInputs::HeavyAttack, FGameplayTag::RequestGameplayTag("Event.Input.Saved.Heavy"));
	}
	else
	{
		SendLocalInputToASC(true, EAbilityInputID::HeavyAttack);
	}
}

void AFTAPlayerController::HandleHeavyAttackActionReleased(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(false, EAbilityInputID::HeavyAttack);

}

void AFTAPlayerController::InputSlowTime(const FInputActionValue& InputActionValue)
{
	if(IsTimeSlowed)
	{
		IsTimeSlowed = false;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	}
	else
	{
		IsTimeSlowed = true;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), .1);
	}
}
