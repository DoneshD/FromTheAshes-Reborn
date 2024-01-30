#include "Characters/PlayableCharacter.h"
#include "Characters/EnemyBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/DamagableInterface.h"
#include "DamageSystem/DamageSystem.h"
#include "DamageSystem/DamageInfo.h"

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

	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));

	InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));
	TimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));

	DamageSystemComponent->AttackTokensCount = 1;
}

void APlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (RotationCurve)
	{
		Timeline->AddInterpFloat(RotationCurve, InterpFunction, FName("Alpha"));
		Timeline->SetTimelinePostUpdateFunc(TimelineFinished);

		Timeline->SetLooping(false);
		Timeline->SetIgnoreTimeDilation(true);
	}
	EnemyReference = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyBase::StaticClass());
}

//------------------------------------------------------------- FSM Resets -----------------------------------------------------------------//

void APlayableCharacter::ResetLightAttack()
{
	LightAttackIndex = 0;
	bLightAttackSaved = false;
}

void APlayableCharacter::ResetHeavyAttack()
{
	HeavyAttackIndex = 0;
	NewHeavyAttackIndex = 0;

	bHeavyAttackSaved = false;
	bHeavyAttackPaused = false;

	ClearHeavyAttackPausedTimer();
}

void APlayableCharacter::ResetAirAttack()
{
	AirComboIndex = 0;
	bLaunched = false;
}

void APlayableCharacter::ResetDodge()
{
	bDodgeSaved = false;
	bCanRoll = false;
	bCanDodge = true;
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
	bSurging = false;
	bSurgeAttackPaused = false;
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
	ResetDodge();
	StopRotation();
	ResetLightAttack();
	ResetHeavyAttack();
	ResetAirAttack();
	ResetCombos();
	ResetSurgeCombo();
}

bool APlayableCharacter::CanAttack()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Execution };
	return !GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying() && !IsStateEqualToAny(MakeArray);
}

bool APlayableCharacter::CanDodge()
{
	TArray<EStates> MakeArray = { EStates::EState_Dodge, EStates::EState_Execution };
	return !GetCharacterMovement()->IsFalling() && bCanDodge && !IsStateEqualToAny(MakeArray);
}

//------------------------------------------------------------- Tick -----------------------------------------------------------------//

void APlayableCharacter::Tick(float DeltaTime)
{

	//------------------------------------------------------------ TICK::Targeting -----------------------------------------------------------------//

	Super::Tick(DeltaTime);
	
	if (bTargeting && HardTarget)
	{
		if (GetDistanceTo(HardTarget) < 2000.f)
		{
			if (GetCharacterMovement()->IsFalling() || GetCharacterMovement()->IsFlying())
			{
				FVector ResultVectorAVector(0, 0, 80.f);
			}

			FVector ResultVector(0, 0, 0);
			FVector TargetLocation = HardTarget->GetActorLocation() - ResultVector;
			FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
			FRotator InterpRot = FMath::RInterpTo(GetControlRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 5.f);

			GetController()->SetControlRotation(InterpRot);
		}
		else
		{
			bTargeting = false;
			HardTarget = NULL;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}

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
		InputComp->BindAction(Input_Jump, ETriggerEvent::Started, this, &APlayableCharacter::DoubleJump);
		InputComp->BindAction(Input_Jump, ETriggerEvent::Completed, this, &APlayableCharacter::StopJump);
		InputComp->BindAction(Input_LookMouse, ETriggerEvent::Triggered, this, &APlayableCharacter::LookMouse);
		InputComp->BindAction(Input_LookStick, ETriggerEvent::Triggered, this, &APlayableCharacter::LookStick);

		InputComp->BindAction(Input_LightAttack, ETriggerEvent::Started, this, &APlayableCharacter::InputLightAttack);
		InputComp->BindAction(Input_HeavyAttack, ETriggerEvent::Started, this, &APlayableCharacter::InputHeavyAttack);
		InputComp->BindAction(Input_Dodge, ETriggerEvent::Started, this, &APlayableCharacter::InputDodge);
		InputComp->BindAction(Input_LockOn, ETriggerEvent::Started, this, &APlayableCharacter::HardLockOn);
		InputComp->BindAction(Input_Parry, ETriggerEvent::Started, this, &APlayableCharacter::InputParry);

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
	AddMovementInput(RightVector, InputDirection.X);
}

void APlayableCharacter::LookMouse(const FInputActionValue& InputValue)
{
	const FVector2D Value = InputValue.Get<FVector2D>();

	AddControllerYawInput(Value.X);

	//Invert depend on controller or mouse
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
	Timeline->Stop();
}


//------------------------------------------------------------- LockOn -----------------------------------------------------------------//

void APlayableCharacter::StopRotation()
{
	Timeline->Stop();
}

void APlayableCharacter::RotationToTarget()
{
	Timeline->PlayFromStart();
}

void APlayableCharacter::SoftLockOn(float Distance)
{
	if (!bTargeting && !HardTarget)
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
	if (!bTargeting && !HardTarget)
	{

		if (UCameraComponent* FollowCamera = this->CameraComp)
		{
			FVector CameraVector = FollowCamera->GetForwardVector();
			FVector EndLocation = (CameraVector * 2000.f) + GetActorLocation();
			FHitResult OutHit;

			TArray<AActor*> ActorArray;
			ActorArray.Add(this);

			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

			bool TargetHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
				GetWorld(),
				GetActorLocation(),
				EndLocation,
				100.f,
				ObjectTypes,
				false,
				ActorArray,
				EDrawDebugTrace::ForDuration,
				OutHit,
				true);

			if (TargetHit)
			{
				AActor* HitActor = OutHit.GetActor();
				
				GetCharacterMovement()->bOrientRotationToMovement = false;
				bTargeting = true;
				HardTarget = HitActor;
			}
		}
	}
	else
	{
		bTargeting = false;
		HardTarget = NULL;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

//------------------------------------------------------------ Dodge -----------------------------------------------------------------//

void APlayableCharacter::EnableRoll()
{
	bCanRoll = true;
}

void APlayableCharacter::DisableRoll()
{
	bCanRoll = false;
}

void APlayableCharacter::EnableDodge()
{
	bCanDodge = true;
}

void APlayableCharacter::DisableDodge()
{
	bCanDodge = false;
}

void APlayableCharacter::DodgeSystem(float X, float Y)
{
	isDodging = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	FVector StartLocation = (GetActorLocation() + GetCharacterMovement()->GetLastInputVector() * 100.f);
	FVector DodgeDirection = (GetCharacterMovement()->GetLastInputVector() * 100.0f) + StartLocation;
	UE_LOG(LogTemp, Warning, TEXT("DodgeDirection: %s"), *DodgeDirection.ToString());
	if (bCanRoll)
	{
		PlayAnimMontage(ForwardRollAnim);
	}
	else
	{
		PlayAnimMontage(ForwardDodgeAnim);

	}
}

void APlayableCharacter::SaveDodge()
{
	if (bDodgeSaved)
	{
		bDodgeSaved = false;
		TArray<EStates> MakeArray = { EStates::EState_Dodge };
		if (!IsStateEqualToAny(MakeArray))
		{
			SetState(EStates::EState_Dodge);
		}
		PerformDodge();
	}
}

void APlayableCharacter::PerformDodge()
{
	StopRotation();
	SoftTarget = NULL;
	SetState(EStates::EState_Dodge);
	if (bTargeting)
	{
		DodgeSystem(InputDirection.X, InputDirection.Y);
	}
	else
	{
		if (bCanRoll)
		{
			PlayAnimMontage(ForwardRollAnim);
		}
		else
		{
			PlayAnimMontage(ForwardDodgeAnim);
		}
	}
}

void APlayableCharacter::InputDodge()
{
	if (CanDodge())
	{
		PerformDodge();
	}
	else
	{
		bDodgeSaved = true;
	}
}

//---------------------------------------------------------- Attack Saves -----------------------------------------------------------------//

void APlayableCharacter::SaveLightAttack()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack };
	if (bLightAttackSaved)
	{
		bLightAttackSaved = false;
		if (IsStateEqualToAny(MakeArray))
		{
			SetState(EStates::EState_Nothing);
		}
		if (bSurgeAttackPaused)
		{
			PerformComboSurge();
		}

		//move inside else if 

		else if (HeavyAttackIndex > 1)
		{
			PerformComboFinisher(ComboExtenderFinishers[3]);
		}
		else if (ComboExtenderIndex > 0)
		{
			if (BranchFinisher)
			{
				PerformComboFinisher(ComboExtenderFinishers[0]);
			}
			else if (HeavyAttackIndex == 0)
			{
				PerformComboExtender(ComboExtenderIndex);
			}
		}
		else
		{
			LightAttack();
		}
	}
}

void APlayableCharacter::SaveHeavyAttack()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack };
	if (bHeavyAttackSaved)
	{
		bHeavyAttackSaved = false;
		//Air Slam()
		if (IsStateEqualToAny(MakeArray))
		{
			SetState(EStates::EState_Nothing);
		}
		if (bHeavyAttackPaused)
		{
			NewHeavyCombo();
		}
		else if (LightAttackIndex == 3)
		{
			PerformComboFinisher(ComboExtenderFinishers[2]);
		}
		else if (LightAttackIndex == 2)
		{
			if(ComboExtenderIndex == 0)
			{
				PerformComboExtender(ComboExtenderIndex);
			}
			else if (BranchFinisher)
			{
				PerformComboFinisher(ComboExtenderFinishers[1]);
			}
		}
		else
		{
			HeavyAttack();
		}
	}
	
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
	bHeavyAttackPaused = true;
	//Not needed, why?
	OnAttackHeavyPausedEvent.Broadcast();
}

void APlayableCharacter::SurgeAttackPaused()
{
	bSurgeAttackPaused = true;
	OnAttackSurgePausedEvent.Broadcast();
}

//------------------------------------------------------ Light Attack Actions -----------------------------------------------------------------//

void APlayableCharacter::PerformLightAttack(int AttackIndex)
{
	UAnimMontage* CurrentMontage = LightAttackCombo[AttackIndex];
	if (CurrentMontage)
	{
		SetState(EStates::EState_Attack);
		SoftLockOn(250.0f);
		PlayAnimMontage(CurrentMontage);
		ResetSurgeCombo();
		LightAttackIndex++;
		if (LightAttackIndex >= LightAttackCombo.Num())
		{
			LightAttackIndex = 0;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Montage"));
	}
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
		//Air attack logic
	}
}

void APlayableCharacter::InputLightAttack()
{
	bDodgeSaved = false;
	bHeavyAttackSaved = false;
	ClearHeavyAttackPausedTimer();

	TArray<EStates> MakeArray = { EStates::EState_Attack };
	if (IsStateEqualToAny(MakeArray))
	{
		bLightAttackSaved = true;
	}
	else
	{
		LightAttack();
	}
}

//--------------------------------------------------------- Heavy Attack Actions -----------------------------------------------------------------//

void APlayableCharacter::PerformHeavyPauseCombo(TArray<TObjectPtr<UAnimMontage>> PausedHeavyAttackCombo)
{
	UAnimMontage* AttackMontage = PausedHeavyAttackCombo[NewHeavyAttackIndex];
	if (AttackMontage)
	{
		SetState(EStates::EState_Attack);
		SoftLockOn(250.0f);
		PlayAnimMontage(AttackMontage);
		NewHeavyAttackIndex++;
		if (NewHeavyAttackIndex >= PausedHeavyAttackCombo.Num())
		{
			NewHeavyAttackIndex = 0;
			bHeavyAttackPaused = false;
		}
	}
}

//TODO: Select combo for scalability
void APlayableCharacter::SelectHeavyPauseCombo()
{
	if (HeavyAttackIndex == 1)
	{
		PerformHeavyPauseCombo(PausedHeavyAttackCombo1);
	}
	//if (HeavyAttackIndex == 2)
	//{
	//	PerformHeavyCombo(PausedHeavyAttackCombo2);
	//}
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
	UAnimMontage* CurrentMontage = HeavyAttackCombo[AttackIndex];
	if (CurrentMontage)
	{
		SetState(EStates::EState_Attack);
		SoftLockOn(500.0f);
		PlayAnimMontage(CurrentMontage);
		StartHeavyAttackPausedTimer();
		if (HeavyAttackIndex == 0)
		{
			StartSurgeAttackPausedTimer();
		}
		else
		{
			ClearSurgeAttackPausedTimer();
			bSurgeAttackPaused = false;
		}
		HeavyAttackIndex++;
		if (HeavyAttackIndex >= HeavyAttackCombo.Num())
		{
			HeavyAttackIndex = 0;
			ClearHeavyAttackPausedTimer();
			bHeavyAttackPaused = false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Montage"));
	}
}

void APlayableCharacter::HeavyAttack()
{
	if (CanAttack())
	{
		ClearHeavyAttackPausedTimer();
		ClearSurgeAttackPausedTimer();
		bHeavyAttackPaused = false;
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
	bDodgeSaved = false;
	bLightAttackSaved = false;

	TArray<EStates> MakeArray = { EStates::EState_Attack };
	if (IsStateEqualToAny(MakeArray))
	{
		bHeavyAttackSaved = true;
	}
	else
	{
		HeavyAttack();
	}
}

//--------------------------------------------------------- Combo Strings -----------------------------------------------------------------//

void APlayableCharacter::PerformComboExtender(int ExtenderIndex)
{
	UAnimMontage* CurrentMontage = ComboExtender[ExtenderIndex];
	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Dodge };
	if (!IsStateEqualToAny(MakeArray))
	{
		if (CurrentMontage)
		{
			bHeavyAttackPaused = false;
			SetState(EStates::EState_Attack);
			SoftLockOn(500.0f);
			ComboExtenderIndex++;
			PlayAnimMontage(CurrentMontage);

			if (ComboExtenderIndex >= ComboExtender.Num())
			{
				BranchFinisher = true;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Montage"));
	}
}

//add parameter to pass anim montage

void APlayableCharacter::PerformComboFinisher(UAnimMontage* FinisherMontage)
{
	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Dodge };
	if (!IsStateEqualToAny(MakeArray))
	{
		if (FinisherMontage)
		{
			bHeavyAttackPaused = false;
			ResetLightAttack();
			ResetHeavyAttack();
			SetState(EStates::EState_Attack);
			SoftLockOn(500.0f);
			PlayAnimMontage(FinisherMontage);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid Montage"));
		}
	}
	else
	{
		return;
	}
}

void APlayableCharacter::PerformComboSurge()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Dodge };

	if (!IsStateEqualToAny(MakeArray))
	{
		bHeavyAttackPaused = false;
		ResetLightAttack();
		ResetHeavyAttack();
		SetState(EStates::EState_Attack);
		SoftLockOn(500.0f);

		PlayAnimMontage((ComboSurgeCount % 2 == 0) ? ComboSurge_L : ComboSurge_R, ComboSurgeSpeed);
		ComboSurgeCount += 1;
		ComboSurgeSpeed = (ComboSurgeCount > 5) ? 1.6 : (ComboSurgeCount > 2) ? 1.4 : 1.2;
	}
	else
	{
		return;
	}
}


//-------------------------------------Parry----------------------------------------

void APlayableCharacter::InputParry()
{

	IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(this);
	if (DamagableInterface)
	{
		if (CanParry && DamagableInterface->WithinParryRange)
		{
			UE_LOG(LogTemp, Warning, TEXT("Parry"));
			PerformParry();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Cant parry"));
		}
	}

}
void APlayableCharacter::PerformParry()
{
	AEnemyBase* ParryTarget = Cast<AEnemyBase>(EnemyReference);
	FTransform ParryMeshTransform = ParryTarget->ParryMesh->GetComponentTransform();

	RootComponent->SetWorldTransform(ParryMeshTransform);

	PlayAnimMontage(ParryTarget->HitParryAnim);
	PlayAnimMontage(ParryAnim);
}
//--------------------------------Damage System-------------------------------------

float APlayableCharacter::NativeGetCurrentHealth()
{
	return DamageSystemComponent->CurrentHealth;
}

float APlayableCharacter::NativeGetMaxHealth()
{
	return DamageSystemComponent->MaxHealth;
}

bool APlayableCharacter::NativeIsDead()
{
	return DamageSystemComponent->IsDead;
}

float APlayableCharacter::NativeHeal(float HealAmount)
{
	return DamageSystemComponent->Heal(HealAmount);
}

bool APlayableCharacter::NativeTakeDamage(FDamageInfo DamageInfo)
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
