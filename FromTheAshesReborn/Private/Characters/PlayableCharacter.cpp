#include "Characters/PlayableCharacter.h"
#include "Characters/EnemyBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/DamagableInterface.h"
#include "DamageSystem/DamageSystem.h"
#include "DamageSystem/DamageInfo.h"
#include "TargetingComponent.h"
#include "ComboSystemComponent.h"
#include "TimelineHelper.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

//------------------------------------------------------------- Constructor -----------------------------------------------------------------//

APlayableCharacter::APlayableCharacter()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 400.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	SpringArmComp->SetUsingAbsoluteRotation(true);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComponent"));
	this->AddOwnedComponent(TargetingComponent);

	ComboSystemComponent = CreateDefaultSubobject<UComboSystemComponent>(TEXT("ComboSystemComponent"));
	this->AddOwnedComponent(ComboSystemComponent);

	//Jump and Air Control
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.7f;
	GetCharacterMovement()->AirControlBoostMultiplier = 6.f;

	//Walk and crouch
	GetCharacterMovement()->SetCrouchedHalfHeight(48.f);
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	RotationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RotationTimeline"));

	DamageSystemComponent->AttackTokensCount = 1;
}

void APlayableCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (RotationCurve)
	{
		RotationTimeline = TimelineHelper::CreateTimeline(RotationTimeline, this, RotationCurve, TEXT("RotationTimeline"), FName("TimelifneFloatReturn"), FName("OnTimelineFinished"));
	}
}

//------------------------------------------------------------- FSM Resets -----------------------------------------------------------------//

void APlayableCharacter::ResetLightAttack()
{
	LightAttackIndex = 0;
	IsLightAttackSaved = false;
}

void APlayableCharacter::ResetHeavyAttack()
{
	HeavyAttackIndex = 0;
	NewHeavyAttackIndex = 0;

	IsHeavyAttackSaved = false;
	IsHeavyAttackPaused = false;

	ClearHeavyAttackPausedTimer();
}

void APlayableCharacter::ResetCombos()
{
	ComboExtenderIndex = 0;
	BranchFinisher = false;
}

void APlayableCharacter::ResetSurgeCombo()
{
	ComboSurgeCount = 0;
	ComboSurgeSpeed = 1.0;
	IsSurging = false;
	IsSurgeAttackPaused = false;
	ClearSurgeAttackPausedTimer();
}

void APlayableCharacter::ResetState()
{
	if (GetCharacterMovement()->IsFalling())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}

	SetState(EStates::EState_Nothing);
	SoftTarget = NULL;
	StopRotation();
	ResetLightAttack();
	ResetHeavyAttack();
	ResetCombos();
	ResetSurgeCombo();
}

bool APlayableCharacter::CanAttack()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Execution };
	return !GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying() && !IsStateEqualToAny(MakeArray);
}

//------------------------------------------------------------- Tick -----------------------------------------------------------------//

void APlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

//--------------------------------------------------------- PlayerInputComponent ---------------------------------------------------------------------//

void APlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		if (LocalPlayer)
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			check(Subsystem);

			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(DefaultInputMapping, 0);
		}
	}

	UEnhancedInputComponent* InputComp = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (InputComp)
	{
		InputComp->BindAction(Input_Move, ETriggerEvent::Triggered, this, &APlayableCharacter::Move);
		InputComp->BindAction(Input_Move, ETriggerEvent::Completed, this, &APlayableCharacter::MoveCanceled);
		InputComp->BindAction(Input_Jump, ETriggerEvent::Started, this, &APlayableCharacter::DoubleJump);
		InputComp->BindAction(Input_Jump, ETriggerEvent::Completed, this, &APlayableCharacter::StopJump);
		InputComp->BindAction(Input_LookMouse, ETriggerEvent::Triggered, this, &APlayableCharacter::LookMouse);
		InputComp->BindAction(Input_LookStick, ETriggerEvent::Triggered, this, &APlayableCharacter::LookStick);

		InputComp->BindAction(Input_LightAttack, ETriggerEvent::Started, this, &APlayableCharacter::InputLightAttack);
		InputComp->BindAction(Input_HeavyAttack, ETriggerEvent::Started, this, &APlayableCharacter::InputHeavyAttack);
		InputComp->BindAction(Input_LockOn, ETriggerEvent::Started, this, &APlayableCharacter::HardLockOn);
	}
}

//------------------------------------------------------------ Movement -----------------------------------------------------------------//
void APlayableCharacter::Move(const FInputActionInstance& Instance)
{
	
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	InputDirection = Instance.GetValue().Get<FVector2D>();
	InputDirection.Normalize();

	InputDirection.X = FMath::Clamp(InputDirection.X, -1.0f, 1.0f);
	InputDirection.Y = FMath::Clamp(InputDirection.Y, -1.0f, 1.0f);

	AddMovementInput(ControlRot.Vector(), InputDirection.Y);

	const FRotationMatrix RotationMatrix(ControlRot);
	const FVector RightVector = RotationMatrix.GetScaledAxis(EAxis::Y);
	HasMovementInput = true;
	AddMovementInput(RightVector, InputDirection.X);
}

void APlayableCharacter::MoveCanceled()
{
	UE_LOG(LogTemp, Warning, TEXT("MoveCanceled"));
	HasMovementInput = false;
}

void APlayableCharacter::LookMouse(const FInputActionValue& InputValue)
{
	const FVector2D Value = InputValue.Get<FVector2D>();

	AddControllerYawInput(Value.X);

	AddControllerPitchInput(-Value.Y);
}

void APlayableCharacter::LookStick(const FInputActionValue& InputValue)
{
	FVector2D Value = InputValue.Get<FVector2D>();

	bool XNegative = Value.X < 0.f;
	bool YNegative = Value.Y < 0.f;

	static const float LookYawRate = 100.0f;
	static const float LookPitchRate = 50.0f;

	Value = Value * Value;

	if (XNegative)
	{
		Value.X *= -1.f;
	}
	if (YNegative)
	{
		Value.Y *= -1.f;
	}

	AddControllerYawInput(Value.X * (LookYawRate)*GetWorld()->GetDeltaSeconds());
	AddControllerPitchInput(Value.Y * (LookPitchRate)*GetWorld()->GetDeltaSeconds());
}
void APlayableCharacter::EnableRootRotation()
{
	if (!SoftTarget && !HardTarget)
	{
		GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
	}
}

void APlayableCharacter::DoubleJump()
{
	if (CanJump())
	{
		Jump();
		JumpCount++;
		if (!GetCharacterMovement()->IsFalling())
		{
			JumpCount = 0;
		}
		else
		{
			if (JumpCount < 2)
			{
				Jump();
			}
		}
	}
}

void APlayableCharacter::StopJump()
{
	StopJumping();
	JumpCount = 0;
}

//------------------------------------------------------------ Timelines -----------------------------------------------------------------//

void APlayableCharacter::TimelineFloatReturn(float value)
{
	if (HardTarget)
	{
		TargetRotateLocation = HardTarget->GetActorLocation();
	}
	else if (SoftTarget)
	{
		TargetRotateLocation = SoftTarget->GetActorLocation();
	}
	else
	{
		StopRotation();
		return;
	}
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetRotateLocation);

	FRotator MakeRotator(TargetRotation.Roll, GetActorRotation().Pitch, TargetRotation.Yaw);
	FRotator InterpRot = FMath::RInterpTo(GetControlRotation(), TargetRotation, value, false);

	SetActorRotation(InterpRot);
}

void APlayableCharacter::OnTimelineFinished()
{
	RotationTimeline->Stop();
}

//------------------------------------------------------------- LockOn -----------------------------------------------------------------//

void APlayableCharacter::StopRotation()
{
	RotationTimeline->Stop();
}

void APlayableCharacter::RotationToTarget()
{
	RotationTimeline->PlayFromStart();
}

void APlayableCharacter::SoftLockOn(float Distance)
{
	if (!IsTargeting && !HardTarget)
	{
		FVector StartLocation = (GetActorLocation() + GetCharacterMovement()->GetLastInputVector() * 100.f);
		FVector EndLocation = (GetCharacterMovement()->GetLastInputVector() * Distance) + StartLocation;
		FHitResult OutHit;

		TArray<AActor*> ActorArray;
		ActorArray.Add(this);
		
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		bool TargetHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
			GetWorld(), 
			StartLocation,
			EndLocation, 
			50.f, 
			ObjectTypes, 
			false, 
			ActorArray, 
			EDrawDebugTrace::None, 
			OutHit, 
			true);

		if (TargetHit)
		{
			AActor* HitActor = OutHit.GetActor();
			if (HitActor)
			{
				SoftTarget = HitActor;
			}
		}
		else
		{
			SoftTarget = NULL;
		}
	}
	
}

void APlayableCharacter::HardLockOn()
{
	TargetingComponent->HardLockOn();
}


//---------------------------------------------------------- Attack Saves -----------------------------------------------------------------//

void APlayableCharacter::SaveLightAttack()
{
	ComboSystemComponent->SaveLightAttack();
}

void APlayableCharacter::SaveHeavyAttack()
{
	ComboSystemComponent->SaveHeavyAttack();
}

//------------------------------------------------------ Pause Attacks -----------------------------------------------------------------//

void APlayableCharacter::StartHeavyAttackPausedTimer()
{
	GetWorldTimerManager().SetTimer(HeavyAttackPauseHandle, this, &APlayableCharacter::HeavyAttackPaused, .8, true);
}

void APlayableCharacter::ClearHeavyAttackPausedTimer()
{
	GetWorldTimerManager().ClearTimer(HeavyAttackPauseHandle);
}

void APlayableCharacter::StartIdleCombatTimer()
{
	GetWorldTimerManager().SetTimer(IdleCombatHandle, this, &APlayableCharacter::ClearIdleCombatTimer, 5, true);
	IsIdleCombat = true;
}

void APlayableCharacter::ClearIdleCombatTimer()
{
	IsIdleCombat = false;
}

void APlayableCharacter::StartSurgeAttackPausedTimer()
{
	GetWorldTimerManager().SetTimer(SurgeAttackPauseHandle, this, &APlayableCharacter::SurgeAttackPaused, .8, true);
}

void APlayableCharacter::ClearSurgeAttackPausedTimer()
{
	GetWorldTimerManager().ClearTimer(SurgeAttackPauseHandle);
}
void APlayableCharacter::HeavyAttackPaused()
{
	IsHeavyAttackPaused = true;
}

void APlayableCharacter::SurgeAttackPaused()
{
	IsSurgeAttackPaused = true;
}

//------------------------------------------------------ Light Attack Actions -----------------------------------------------------------------//

void APlayableCharacter::PerformLightAttack(int AttackIndex)
{
	ComboSystemComponent->PerformLightAttack(AttackIndex);
}

void APlayableCharacter::LightAttack()
{
	if (CanAttack())
	{
		ResetHeavyAttack();
		PerformLightAttack(LightAttackIndex);
	}
	else
	{
		return;
	}
}

void APlayableCharacter::InputLightAttack()
{
	IsHeavyAttackSaved = false;
	StartIdleCombatTimer();

	ClearHeavyAttackPausedTimer();
	TArray<EStates> MakeArray = { EStates::EState_Attack };
	if (IsStateEqualToAny(MakeArray))
	{
		IsLightAttackSaved = true;
	}
	else
	{
		LightAttack();
	}
}

//--------------------------------------------------------- Heavy Attack Actions -----------------------------------------------------------------//

void APlayableCharacter::PerformHeavyPauseCombo(TArray<TObjectPtr<UAnimMontage>> PausedHeavyAttackCombo)
{
	ComboSystemComponent->PerformHeavyPauseCombo(PausedHeavyAttackCombo);
}

void APlayableCharacter::SelectHeavyPauseCombo()
{
	if (HeavyAttackIndex == 1)
	{
		PerformHeavyPauseCombo(PausedHeavyAttackCombo1);
	}
}
void APlayableCharacter::NewHeavyCombo()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack };
	if(!IsStateEqualToAny(MakeArray))
	{
		SelectHeavyPauseCombo();
	}
}

void APlayableCharacter::PerformHeavyAttack(int AttackIndex)
{
	ComboSystemComponent->PerformHeavyAttack(AttackIndex);
}

void APlayableCharacter::HeavyAttack()
{
	if (CanAttack())
	{
		ClearHeavyAttackPausedTimer();
		ClearSurgeAttackPausedTimer();
		IsHeavyAttackPaused = false;
		ResetLightAttack();
		PerformHeavyAttack(HeavyAttackIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error"));
	}
}

void APlayableCharacter::InputHeavyAttack()
{
	IsLightAttackSaved = false;
	StartIdleCombatTimer();
	TArray<EStates> MakeArray = { EStates::EState_Attack };
	if (IsStateEqualToAny(MakeArray))
	{
		IsHeavyAttackSaved = true;
	}
	else
	{
		HeavyAttack();
	}
}

//--------------------------------------------------------- Combo Strings -----------------------------------------------------------------//

void APlayableCharacter::PerformComboExtender(int ExtenderIndex)
{
	ComboSystemComponent->PerformComboExtender(ExtenderIndex);
}

void APlayableCharacter::PerformComboFinisher(UAnimMontage* FinisherMontage)
{
	ComboSystemComponent->PerformComboFinisher(FinisherMontage);
}

void APlayableCharacter::PerformComboSurge()
{
	ComboSystemComponent->PerformComboSurge();
}

//--------------------------------Damage System-------------------------------------

float APlayableCharacter::GetCurrentHealth()
{
	return DamageSystemComponent->CurrentHealth;
}

float APlayableCharacter::GetMaxHealth()
{
	return DamageSystemComponent->MaxHealth;
}

bool APlayableCharacter::IsDead()
{
	return DamageSystemComponent->IsDead;
}

float APlayableCharacter::Heal(float HealAmount)
{
	return DamageSystemComponent->Heal(HealAmount);
}

bool APlayableCharacter::TakeDamage(FDamageInfo DamageInfo)
{
	return DamageSystemComponent->TakeDamage(DamageInfo);
}

bool APlayableCharacter::ReserveAttackToken(int Amount)
{
	return DamageSystemComponent->ReserveAttackTokens(Amount);
}

void APlayableCharacter::ReturnAttackToken(int Amount)
{
	DamageSystemComponent->ReturnAttackTokens(Amount);
}
