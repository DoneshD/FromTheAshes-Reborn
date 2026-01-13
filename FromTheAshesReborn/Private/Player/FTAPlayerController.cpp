#include "Player/FTAPlayerController.h"
#include "Player/FTAPlayerState.h"
#include "Player/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "Input/FTAInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagContainer.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
#include "TargetingSystem/TargetingSystemComponent.h"

AFTAPlayerController::AFTAPlayerController(const FObjectInitializer& ObjectInitializer)
{
	
}

UFTAAbilitySystemComponent* AFTAPlayerController::GetFTAAbilitySystemComponent() const
{
	AFTAPlayerState* FTAPlayerState = CastChecked<AFTAPlayerState>(PlayerState);
	check(FTAPlayerState)

	return FTAPlayerState->GetFTAAbilitySystemComponent();
}

AFTAPlayerState* AFTAPlayerController::GetFTAPlayerState()
{
	return CastChecked<AFTAPlayerState>(PlayerState);
}

void AFTAPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void AFTAPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (InPawn->InputComponent != nullptr)
	{
		InitializePlayerInput(InPawn->InputComponent);
	}
}

void AFTAPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	if(EnhancedInputComponent)
	{
		EnhancedInputComponent->ClearActionBindings();
	}
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

	float WalkSpeed = InputActionValue.GetMagnitude() * 20.0f;
	float ClampedWalkSpeed = FMath::Clamp(WalkSpeed, 200, 1000);

	if(ClampedWalkSpeed < 600.0f)
	{
		PlayerCharacter->SetMaxWalkSpeed(200.0f);
	}
	else
	{
		PlayerCharacter->SetMaxWalkSpeed(1000.0f);
	}

	//Move to controller
	PlayerCharacter->HasMovementInput = true;

	SendLocalInputToASC(true, EAbilityInputID::Move);

}

void AFTAPlayerController::HandleMoveActionReleased(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(false, EAbilityInputID::Move);
	
	//Move to controller
	PlayerCharacter->HasMovementInput = false;
	
}

void AFTAPlayerController::HandleInputLookMouse(const FInputActionValue& InputActionValue)
{
	LookAxisVector = InputActionValue.Get<FVector2D>();

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);

	//This is costly, refactor later
	UTargetingSystemComponent* TargetingSystemComponent = GetPawn()->FindComponentByClass<UTargetingSystemComponent>();
	if(!TargetingSystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("AFTAPlayerController::HandleInputLookMouse - TargetSystemComponent is NULL"));
		return;
	}
	if(!TargetingSystemComponent->GetLockedOnTargetActor())
	{
		return;
	}
	// TargetingSystemComponent->TargetActorWithAxisInput(LookAxisVector.X);
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

void AFTAPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	AFTAPlayerState* FTAPlayerState = GetPlayerState<AFTAPlayerState>();
	if(!FTAPlayerState) { return; }
	UFTAAbilitySystemComponent* AbilitySystemComponent = CastChecked<UFTAAbilitySystemComponent>(FTAPlayerState->GetAbilitySystemComponent());
	
	AbilitySystemComponent->ProcessAbilityInput(DeltaTime, bGamePaused);

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AFTAPlayerController::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	
	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	if(!PlayerCharacter) { return; }
	
	// Use custom input component instead
	
	EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] AFTAPlayerController::InitializePlayerInput - EnhancedInputComponent is null"), *GetActorNameOrLabel());
		return;
	}
	
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(!InputSubsystem) { return; }
	check(InputSubsystem);

	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(DefaultInputMappingContext, 0);

	UFTAInputComponent* FTAInputComponent = NewObject<UFTAInputComponent>(PlayerInputComponent);

	if (!FTAInputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] AFTAPlayerController::InitializePlayerInput - FTAInputComponent is null"), *GetActorNameOrLabel());
		return;
	}
	TArray<uint32> BindHandles;
	FTAInputComponent->BindAbilityAction(FTAInputConfig, this, &AFTAPlayerController::InputAbilityInputTagPressed, &AFTAPlayerController::InputAbilityInputTagReleased, /*out*/ BindHandles);
	
	FTAInputComponent->BindNativeAction(FTAInputConfig, FGameplayTag::RequestGameplayTag("InputTag.Pressed.Move"), ETriggerEvent::Triggered, this, &AFTAPlayerController::HandleMoveActionPressed);
	FTAInputComponent->BindNativeAction(FTAInputConfig, FGameplayTag::RequestGameplayTag("InputTag.Released.Move"), ETriggerEvent::Completed, this, &AFTAPlayerController::HandleMoveActionReleased);
	FTAInputComponent->BindNativeAction(FTAInputConfig,  FGameplayTag::RequestGameplayTag("InputTag.Pressed.Look.Mouse"), ETriggerEvent::Triggered, this, &AFTAPlayerController::HandleInputLookMouse);
	
}

void AFTAPlayerController::InputAbilityInputTagPressed(FGameplayTag InputTag)
{
	bool IsTagValid = UTagValidationFunctionLibrary::IsRegisteredGameplayTag(InputTag);
	if(!IsTagValid)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] AFTAPlayerController: InputAbilityInputTagPressed - Tag(s) is invalid"), *GetActorNameOrLabel());
		return;
	}
	AFTAPlayerState* FTAPlayerState = GetPlayerState<AFTAPlayerState>();
	
	if(!FTAPlayerState)
	{
		return;
	}
	
	UFTAAbilitySystemComponent* FTAAbilitySystemComponent = CastChecked<UFTAAbilitySystemComponent>(FTAPlayerState->GetAbilitySystemComponent());
	FTAAbilitySystemComponent->AbilityInputTagPressed(InputTag);
}

void AFTAPlayerController::InputAbilityInputTagReleased(FGameplayTag InputTag)
{
	AFTAPlayerState* FTAPlayerState = GetPlayerState<AFTAPlayerState>();
	
	if(!FTAPlayerState)
	{
		return;
	}
	UFTAAbilitySystemComponent* FTAAbilitySystemComponent = CastChecked<UFTAAbilitySystemComponent>(FTAPlayerState->GetAbilitySystemComponent());
	FTAAbilitySystemComponent->AbilityInputTagReleased(InputTag);
}