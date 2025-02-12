#include "Player/FTAPlayerController.h"
#include "Player/FTAPlayerState.h"
#include "Player/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TargetSystemComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "Input/FTAInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagContainer.h"
#include "GameplayTags/FTAGameplayTags.h"
#include "ParkourSystem/ParkourSystemComponent.h"

AFTAPlayerController::AFTAPlayerController(const FObjectInitializer& ObjectInitializer)
{
	
}

UFTAAbilitySystemComponent* AFTAPlayerController::GetFTAAbilitySystemComponent() const
{
	AFTAPlayerState* FTAPlayerState = CastChecked<AFTAPlayerState>(PlayerState);
	check(FTAPlayerState)

	return FTAPlayerState->GetFTAAbilitySystemComponent();
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

	//This is costly, refactor later
	UTargetSystemComponent* TargetSystemComponent = GetPawn()->FindComponentByClass<UTargetSystemComponent>();
	if(!TargetSystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetSystemComponent is NULL"));
		return;
	}
	if(!TargetSystemComponent->GetLockedOnTargetActor())
	{
		return;
	}
	TargetSystemComponent->TargetActorWithAxisInput(LookAxisVector.X);
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
	if(!EnhancedInputComponent) { return; }
	
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(!InputSubsystem) { return; }
	check(InputSubsystem);

	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(DefaultInputMappingContext, 0);

	UFTAInputComponent* FTAInputComponent = NewObject<UFTAInputComponent>(PlayerInputComponent);

	if(!FTAInputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("FTAInputComponent is NULL"));
		return;
	}
	TArray<uint32> BindHandles;
	FTAInputComponent->BindAbilityAction(FTAInputConfig, this, &AFTAPlayerController::InputAbilityInputTagPressed, &AFTAPlayerController::InputAbilityInputTagReleased, /*out*/ BindHandles);
	
	FTAInputComponent->BindNativeAction(FTAInputConfig, FGameplayTag::RequestGameplayTag("InputTag.Pressed.Move"), ETriggerEvent::Triggered, this, &AFTAPlayerController::HandleMoveActionPressed);
	FTAInputComponent->BindNativeAction(FTAInputConfig,  FGameplayTag::RequestGameplayTag("InputTag.Pressed.Look.Mouse"), ETriggerEvent::Triggered, this, &AFTAPlayerController::HandleInputLookMouse);
	
}

void AFTAPlayerController::InputAbilityInputTagPressed(FGameplayTag InputTag)
{
	AFTAPlayerState* FTAPlayerState = GetPlayerState<AFTAPlayerState>();
	
	if(!FTAPlayerState) { return; }
	
	UFTAAbilitySystemComponent* AbilitySystemComponent = CastChecked<UFTAAbilitySystemComponent>(FTAPlayerState->GetAbilitySystemComponent());
	AbilitySystemComponent->AbilityInputTagPressed(InputTag);
}

void AFTAPlayerController::InputAbilityInputTagReleased(FGameplayTag InputTag)
{
	AFTAPlayerState* FTAPlayerState = GetPlayerState<AFTAPlayerState>();
	
	if(!FTAPlayerState) { return; }
	UFTAAbilitySystemComponent* AbilitySystemComponent = CastChecked<UFTAAbilitySystemComponent>(FTAPlayerState->GetAbilitySystemComponent());
	AbilitySystemComponent->AbilityInputTagReleased(InputTag);
}