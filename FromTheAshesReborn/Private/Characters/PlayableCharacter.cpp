#include "Characters/PlayableCharacter.h"
#include "Characters/EnemyBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Interfaces/DamagableInterface.h"
#include "DamageSystem/DamageSystem.h"
#include "DamageSystem/DamageInfo.h"
#include "CombatComponents/ComboSystemComponent.h"
#include "CombatComponents/MeleeAttackLogicComponent.h"
#include "MovementComponents/DashSystemComponent.h"
#include "TargetingComponents/TargetingSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Helpers/TimelineHelper.h"
#include "Weapons/MeleeWeapon.h"
#include "LockOnSphere.h"
#include "MotionWarpingComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

//------------------------------------------------------------- Constructor -----------------------------------------------------------------//

APlayableCharacter::APlayableCharacter()
{

	LockOnSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LockOnSphere"));
	LockOnSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(LockOnSphere);

	SpringArmComp->TargetArmLength = 400.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	SpringArmComp->SetUsingAbsoluteRotation(true);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	ComboSystemComponent = CreateDefaultSubobject<UComboSystemComponent>(TEXT("ComboSystemComponent"));
	this->AddOwnedComponent(ComboSystemComponent);

	MeleeAttackLogicComponent = CreateDefaultSubobject<UMeleeAttackLogicComponent>(TEXT("MeleeAttackLogicComponent"));
	this->AddOwnedComponent(MeleeAttackLogicComponent);

	DashSystemComponent = CreateDefaultSubobject<UDashSystemComponent>(TEXT("DashSystemComponent"));
	this->AddOwnedComponent(DashSystemComponent);

	TargetingSystemComponent = CreateDefaultSubobject<UTargetingSystemComponent>(TEXT("TargetingSystemComponent"));
	this->AddOwnedComponent(TargetingSystemComponent);

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
		RotationTimeline = TimelineHelper::CreateTimeline(RotationTimeline, this, RotationCurve, TEXT("RotationTimeline"), FName("TimelineFloatReturn"), FName("OnTimelineFinished"));
	}

	if (MeleeWeapon_L = GetWorld()->SpawnActor<AMeleeWeapon>(MeleeWeaponClass, GetMesh()->GetSocketTransform(TEXT("hand_l_player_weapon_socket"))))
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
		MeleeWeapon_L->AttachToComponent(GetMesh(), AttachmentRules, TEXT("hand_l_player_weapon_socket"));
	}

	if (MeleeWeapon_R = GetWorld()->SpawnActor<AMeleeWeapon>(MeleeWeaponClass, GetMesh()->GetSocketTransform(TEXT("hand_r_player_weapon_socket"))))
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
		MeleeWeapon_R->AttachToComponent(GetMesh(), AttachmentRules, TEXT("hand_r_player_weapon_socket"));
	}

	AActor* CombatManagerInstance = UGameplayStatics::GetActorOfClass(GetWorld(), CombatManagerClass);
	CombatManager = Cast<ACombatManager>(CombatManagerInstance);

	DamageSystemComponent->OnDeathResponse.BindUObject(this, &APlayableCharacter::HandleDeath);
	DamageSystemComponent->OnDamageResponse.AddUObject(this, &APlayableCharacter::HandleHitReaction);
}

void APlayableCharacter::InputSlowTime()
{
	if (IsSlowTime)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0);
		IsSlowTime = false;
	}
	else if (!IsSlowTime)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1);
		IsSlowTime = true;
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
	DashSystemComponent->IsDashSaved = false;
	DashSystemComponent->AlreadyDashed = false;
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

bool APlayableCharacter::CanDash()
{
	TArray<EStates> MakeArray = { EStates::EState_Dash, EStates::EState_Execution };
	return !GetCharacterMovement()->IsFalling() && !IsStateEqualToAny(MakeArray);
}

bool APlayableCharacter::CanJump()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Dash };
	return !IsStateEqualToAny(MakeArray);
}

void APlayableCharacter::SaveDash()
{
	DashSystemComponent->SaveDash();
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
		InputComp->BindAction(Input_Move, ETriggerEvent::Started, this, &APlayableCharacter::StartMove);
		InputComp->BindAction(Input_Move, ETriggerEvent::Completed, this, &APlayableCharacter::MoveCanceled);
		InputComp->BindAction(Input_Jump, ETriggerEvent::Started, this, &APlayableCharacter::DoubleJump);
		InputComp->BindAction(Input_Jump, ETriggerEvent::Completed, this, &APlayableCharacter::StopJump);
		InputComp->BindAction(Input_LookMouse, ETriggerEvent::Triggered, this, &APlayableCharacter::LookMouse);
		InputComp->BindAction(Input_LookStick, ETriggerEvent::Triggered, this, &APlayableCharacter::LookStick);

		InputComp->BindAction(Input_LightAttack, ETriggerEvent::Started, this, &APlayableCharacter::InputLightAttack);
		InputComp->BindAction(Input_HeavyAttack, ETriggerEvent::Started, this, &APlayableCharacter::InputHeavyAttack);
		InputComp->BindAction(Input_Dash, ETriggerEvent::Started, this, &APlayableCharacter::InputDash);
		InputComp->BindAction(Input_LockOn, ETriggerEvent::Started, this, &APlayableCharacter::InputLockOn);

		//Debug purposes
		InputComp->BindAction(Input_SlowTime, ETriggerEvent::Started, this, &APlayableCharacter::InputSlowTime);

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


void APlayableCharacter::StartMove()
{
	GetWorldTimerManager().SetTimer(IsSprintingTimerHandle, this, &APlayableCharacter::StartSprintTimer, 1, true);
}

void APlayableCharacter::MoveCanceled()
{
	HasMovementInput = false;
	IsSprinting = false;
	GetWorldTimerManager().ClearTimer(IsSprintingTimerHandle);
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

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

void APlayableCharacter::InputLockOn()
{
	if (TargetingSystemComponent)
	{
		TargetingSystemComponent->HardLockOn();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: No TargetingSystemComponent"));
	}
}

//----------------------------------------------------------- Dash Actions -----------------------------------------------------------------//

void APlayableCharacter::InputDash()
{
	if (CanDash())
	{
		SetState(EStates::EState_Dash);

		if (TargetingSystemComponent->HardTarget)
		{
			DashSystemComponent->LockOnDash();
		}
		else
		{
			DashSystemComponent->FreeLockDash();
		}
	}
	else
	{
		DashSystemComponent->IsDashSaved = true;
	}
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
	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Dash };
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
	if (!IsStateEqualToAny(MakeArray))
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

void APlayableCharacter::StartSprintTimer()
{
	IsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
}

void APlayableCharacter::ClearSprintTimer()
{
	
	IsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
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

void APlayableCharacter::HandleDeath()
{
	InputComponent->ClearActionBindings();
	InputComponent->AxisBindings.Empty();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Remove controls
	PlayAnimMontage(DeathMontage);
	CombatManager->HandleDeath(this);

}

void APlayableCharacter::HandleHitReaction(FDamageInfo DamageInfo)
{
	GetCharacterMovement()->StopMovementImmediately();

	UAnimMontage* HitReactionMontage = nullptr;

	switch (DamageInfo.FacingDirection)
	{
	case EFacingDirection::EFacingDirection_Left:
		HitReactionMontage = RightHitReaction;
		break;

	case EFacingDirection::EFacingDirection_Right:
		HitReactionMontage = LeftHitReaction;
		break;

	case EFacingDirection::EFacingDirection_Front:
		HitReactionMontage = FrontHitReaction;
		break;

	case EFacingDirection::EFacingDirection_Back:
		HitReactionMontage = BackHitReaction;
		break;

	case EFacingDirection::EFacingDirection_FrontLeft:
		HitReactionMontage = FrontHitReaction;
		break;

	case EFacingDirection::EFacingDirection_FrontRight:
		HitReactionMontage = FrontHitReaction;
		break;

	case EFacingDirection::EFacingDirection_BackLeft:
		HitReactionMontage = BackHitReaction;
		break;

	case EFacingDirection::EFacingDirection_BackRight:
		HitReactionMontage = BackHitReaction;
		break;

	default:
		break;
	}

	if (HitReactionMontage)
	{
		PlayAnimMontage(HitReactionMontage);
	}
}

AMeleeWeapon* APlayableCharacter::GetLeftWeapon()
{
	return MeleeWeapon_L;
}

AMeleeWeapon* APlayableCharacter::GetRightWeapon()
{
	return MeleeWeapon_R;
}

void APlayableCharacter::EmptyHitActorsArray()
{
	MeleeAttackLogicComponent->EmptyHitActorsArray();
}

void APlayableCharacter::StartMeleeAttackCollisions()
{
	MeleeAttackLogicComponent->StartMeleeAttackCollisions();
}

void APlayableCharacter::EndMeleeAttackCollisions()
{
	MeleeAttackLogicComponent->EndMeleeAttackCollisions();
}

void APlayableCharacter::MeleeTraceCollisions()
{

	MeleeAttackLogicComponent->MeleeTraceCollisions();
}

bool APlayableCharacter::MeleeWeaponSphereTrace(FVector StartLocation, FVector EndLocation, TArray<FHitResult>& Hits)
{
	return MeleeAttackLogicComponent->MeleeWeaponSphereTrace(StartLocation, EndLocation, Hits);
}

void APlayableCharacter::MeleeAttackWarpToTarget()
{
	MeleeAttackLogicComponent->MeleeAttackWarpToTarget(MeleeAttackLogicComponent->MeleeAttackRange, HasMovementInput);
}

void APlayableCharacter::ResetMeleeAttackWarpToTarget()
{
	MeleeAttackLogicComponent->ResetMeleeAttackWarpToTarget();
}

void APlayableCharacter::DashWarpToTarget()
{
	DashSystemComponent->DashWarpToTarget();
}

void APlayableCharacter::ResetDashWarpToTarget()
{
	DashSystemComponent->ResetDashWarpToTarget();
}

int APlayableCharacter::GetMaxAttackersCount()
{
	return 4;
}


