#include "Player/FTAPlayerController.h"
#include "Player/FTAPlayerState.h"
#include "Player/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TargetSystemComponent.h"
#include "Player/PlayerComboManagerComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedHeavyMeleeAttack.h"
#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedLightMeleeAttack.h"
#include "Input/FTAInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagContainer.h"
#include "GameplayTags/FTAGameplayTags.h"
#include "ParkourSystem/ParkourSystemComponent.h"

void AFTAPlayerController::ProcessAbilityComboData(UGameplayAbility* Ability)
{
	UFTAGameplayAbility* FTAGameplayAbility = Cast<UFTAGameplayAbility>(Ability);
	if (FTAGameplayAbility && PlayerComboManager)
	{
		PlayerComboManager->AbilityComboDataArray.Add(FTAGameplayAbility->AbilityComboDataStruct);
		OnRegisterWindowTagEventDelegate.Broadcast(FTAGameplayAbility->AbilityComboDataStruct);
	}
}

UGameplayAbility* AFTAPlayerController::GetAbilityForInput(EAllowedInputs InputType)
{
	for (FGameplayAbilitySpec& Spec : GetFTAAbilitySystemComponent()->GetActivatableAbilities())
	{
		switch (InputType)
		{
		case EAllowedInputs::LightAttack:
			if (Spec.Ability->IsA(UGA_GroundedLightMeleeAttack::StaticClass()))
			{
				return Spec.Ability;
			}
			
		case EAllowedInputs::HeavyAttack:
			if (Spec.Ability->IsA(UGA_GroundedHeavyMeleeAttack::StaticClass()))
			{
				return Spec.Ability;
			}
			
		case EAllowedInputs::Dash:
			if (Spec.InputID == 6)
				return Spec.Ability;
			break;

		default:
			break;
		}
	}
	return nullptr;
}

AFTAPlayerController::AFTAPlayerController(const FObjectInitializer& ObjectInitializer)
{
	
}

UFTAAbilitySystemComponent* AFTAPlayerController::GetFTAAbilitySystemComponent() const
{
	AFTAPlayerState* FTAPlayerState = CastChecked<AFTAPlayerState>(PlayerState);
	check(FTAPlayerState)

	return FTAPlayerState->GetFTAAbilitySystemComponent();
}

void AFTAPlayerController::InputQueueUpdateAllowedInputsBegin(TArray<EAllowedInputs> AllowedInputs)
{
	CurrentAllowedInputs = AllowedInputs;
	IsInInputQueueWindow = true;

	for (const EAllowedInputs& AllowedInputElement : AllowedInputs)
	{
		UGameplayAbility* Ability = GetAbilityForInput(AllowedInputElement);
		if (!Ability) continue;
		
		ProcessAbilityComboData(Ability);
	}
}

void AFTAPlayerController::InputQueueUpdateAllowedInputsEnd(TArray<EAllowedInputs> AllowedInputs)
{
	IsInInputQueueWindow = false;
	CurrentAllowedInputs.Empty();
	LastInputSavedTag = FGameplayTag::RequestGameplayTag("Event.Input.Saved.None");
	AllowedInputs.Empty();
}

void AFTAPlayerController::AddInputToQueue(EAllowedInputs InputToQueue, FGameplayTag SavedInputTag)
{
	if(IsInInputQueueWindow)
	{
		if(CurrentAllowedInputs.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("No CurrentAllowedInputs"));
			return;
		}
		
		if(CurrentAllowedInputs.Contains(InputToQueue))
		{
			if(!GetFTAAbilitySystemComponent()->HasMatchingGameplayTag(SavedInputTag))
			{
				LastInputSavedTag = SavedInputTag;
			}
		}
	}
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

	// PlayerCharacter = Cast<APlayerCharacter>(InPawn);
	// if(!PlayerCharacter) { return; }
	//
	// AFTAPlayerState* PS = GetPlayerState<AFTAPlayerState>();
	// if (PS)
	// {
	// 	// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
	// 	ASC = PS->GetAbilitySystemComponent();
	// 	ASC->InitAbilityActorInfo(PS, InPawn);
	// }
	//
	// EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	// if(!EnhancedInputComponent) { return; }
	//
	// UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	// if(!InputSubsystem) { return; }
	// check(InputSubsystem);
	//
	// InputSubsystem->ClearAllMappings();
	// InputSubsystem->AddMappingContext(DefaultInputMappingContext, 0);
	//
	// EnhancedInputComponent->BindAction(Input_Move, ETriggerEvent::Triggered, this, &AFTAPlayerController::HandleMoveActionPressed);
	// EnhancedInputComponent->BindAction(Input_Move, ETriggerEvent::Completed, this, &AFTAPlayerController::HandleMoveActionReleased);
	//
	// EnhancedInputComponent->BindAction(Input_LookMouse, ETriggerEvent::Triggered, this, &AFTAPlayerController::HandleInputLookMouse);
	//
	// EnhancedInputComponent->BindAction(Input_LightAttack, ETriggerEvent::Started, this, &AFTAPlayerController::HandleLightAttackActionPressed);
	// EnhancedInputComponent->BindAction(Input_LightAttack, ETriggerEvent::Completed, this, &AFTAPlayerController::HandleLightAttackActionReleased);
	//
	// EnhancedInputComponent->BindAction(Input_HeavyAttack, ETriggerEvent::Started, this, &AFTAPlayerController::HandleHeavyAttackActionPressed);
	// EnhancedInputComponent->BindAction(Input_HeavyAttack, ETriggerEvent::Completed, this, &AFTAPlayerController::HandleHeavyAttackActionReleased);
	//
	// EnhancedInputComponent->BindAction(Input_Jump, ETriggerEvent::Started, this, &AFTAPlayerController::HandleJumpActionPressed);
	// EnhancedInputComponent->BindAction(Input_Jump, ETriggerEvent::Completed, this, &AFTAPlayerController::HandleJumpActionReleased);
	//
	// EnhancedInputComponent->BindAction(Input_Dash, ETriggerEvent::Started, this, &AFTAPlayerController::HandleDashActionPressed);
	// EnhancedInputComponent->BindAction(Input_Dash, ETriggerEvent::Completed, this, &AFTAPlayerController::HandleDashActionReleased);
	//
	// EnhancedInputComponent->BindAction(Input_LockOn, ETriggerEvent::Started, this, &AFTAPlayerController::HandleLockOnActionPressed);
	// EnhancedInputComponent->BindAction(Input_LockOn, ETriggerEvent::Completed, this, &AFTAPlayerController::HandleLockOnActionReleased);
	//
	// EnhancedInputComponent->BindAction(Input_Vault, ETriggerEvent::Started, this, &AFTAPlayerController::HandleVaultActionPressed);
	// EnhancedInputComponent->BindAction(Input_Vault, ETriggerEvent::Completed, this, &AFTAPlayerController::HandleVaultActionReleased);
	//
	// //Debug purposes
	// EnhancedInputComponent->BindAction(Input_SlowTime, ETriggerEvent::Started, this, &AFTAPlayerController::InputSlowTime);
	//
	// PlayerComboManager = PlayerCharacter->FindComponentByClass<UPlayerComboManagerComponent>();
	
	// if (PlayerComboManager)
	// {
	// 	OnRegisterWindowTagEventDelegate.AddUniqueDynamic(PlayerComboManager, &UPlayerComboManagerComponent::RegisterGameplayTagEvent);
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("PlayerComboManager NOT FOUND"));
	// }
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

void AFTAPlayerController::HandleJumpActionPressed(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(true, EAbilityInputID::Jump);
}

void AFTAPlayerController::HandleJumpActionReleased(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(false, EAbilityInputID::Jump);
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

void AFTAPlayerController::HandleDashActionPressed(const FInputActionValue& InputActionValue)
{
	// if(IsInInputQueueWindow)
	// {
	// 	AddInputToQueue(EAllowedInputs::Dash, FGameplayTag::RequestGameplayTag("Event.Input.Saved.Dash"));
	// }
	// else
	// {
	// 	SendLocalInputToASC(true, EAbilityInputID::Dash);
	// }
}

void AFTAPlayerController::HandleDashActionReleased(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(false, EAbilityInputID::Dash);
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
	
	FTAInputComponent->BindNativeAction(FTAInputConfig, FTAGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &AFTAPlayerController::HandleMoveActionPressed);
	FTAInputComponent->BindNativeAction(FTAInputConfig, FTAGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &AFTAPlayerController::HandleInputLookMouse);

	PlayerComboManager = PlayerCharacter->FindComponentByClass<UPlayerComboManagerComponent>();
	
	if (PlayerComboManager)
	{
		OnRegisterWindowTagEventDelegate.AddUniqueDynamic(PlayerComboManager, &UPlayerComboManagerComponent::RegisterGameplayTagEvent);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerComboManager NOT FOUND"));
	}
}

void AFTAPlayerController::InputAbilityInputTagPressed(FGameplayTag InputTag)
{
	// TempAbilitySystemComponent->AbilityInputTagPressed(InputTag);
	AFTAPlayerState* FTAPlayerState = GetPlayerState<AFTAPlayerState>();
	
	if(!FTAPlayerState) { return; }
	
	UFTAAbilitySystemComponent* AbilitySystemComponent = CastChecked<UFTAAbilitySystemComponent>(FTAPlayerState->GetAbilitySystemComponent());
	AbilitySystemComponent->AbilityInputTagPressed(InputTag);
}

void AFTAPlayerController::InputAbilityInputTagReleased(FGameplayTag InputTag)
{
	// TempAbilitySystemComponent->AbilityInputTagReleased(InputTag);
	AFTAPlayerState* FTAPlayerState = GetPlayerState<AFTAPlayerState>();
	
	if(!FTAPlayerState) { return; }
	
	UFTAAbilitySystemComponent* AbilitySystemComponent = CastChecked<UFTAAbilitySystemComponent>(FTAPlayerState->GetAbilitySystemComponent());
	AbilitySystemComponent->AbilityInputTagReleased(InputTag);
}

void AFTAPlayerController::HandleLockOnActionPressed(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(true, EAbilityInputID::LockOn);
}

void AFTAPlayerController::HandleLockOnActionReleased(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(false, EAbilityInputID::LockOn);
}

void AFTAPlayerController::HandleVaultActionPressed(const FInputActionValue& InputActionValue)
{
	UParkourSystemComponent* ParkourSystemComponent = GetPawn()->FindComponentByClass<UParkourSystemComponent>();
	if(!ParkourSystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Parkour Component is NULL"));
	}
	ParkourSystemComponent->ParkourInputPressedVault();
}

void AFTAPlayerController::HandleVaultActionReleased(const FInputActionValue& InputActionValue)
{
	
}
