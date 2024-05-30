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
#include "CombatComponents/GroundedComboStringComponent.h"
#include "CombatComponents/MeleeAttackLogicComponent.h"
#include "MovementComponents/DashSystemComponent.h"
#include "TargetingComponents/TargetingSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Helpers/TimelineHelper.h"
#include "Weapons/MeleeWeapon.h"
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


	GroundedComboStringComponent = CreateDefaultSubobject<UGroundedComboStringComponent>(TEXT("GroundedComboStringComponent"));
	this->AddOwnedComponent(GroundedComboStringComponent);

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

	if (LockOnSphere)
	{
		InitialSphereLocation = LockOnSphere->GetRelativeLocation();
	}

	if (RotationCurve)
	{
		RotationTimeline = TimelineHelper::CreateTimeline(RotationTimeline, this, RotationCurve, TEXT("RotationTimeline"), FName("TimelineFloatReturn"), FName("OnTimelineFinished"));
	}

	if (MeleeWeapon_R = GetWorld()->SpawnActor<AMeleeWeapon>(LightMeleeWeaponClass, GetMesh()->GetSocketTransform(TEXT("hand_r_player_weapon_socket"))))
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


void APlayableCharacter::ResetState()
{
	if (GetCharacterMovement()->IsFalling())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
	DashSystemComponent->IsDashSaved = false;
	SetState(EStates::EState_Nothing);
	SoftTarget = NULL;
	DestroyLeftWeapon();
	StopRotation();

	ResetComboString();

}

void APlayableCharacter::ResetComboString()
{
	GroundedComboStringComponent->CurrentAttackIndex = 0;
	GroundedComboStringComponent->CurrentComboString = TEXT("");
	GroundedComboStringComponent->IsAttackPaused = false;
	GroundedComboStringComponent->ClearAttackPauseTimer();
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
	DashSystemComponent->SelectBlink();
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
		DashSystemComponent->PerformDash();
	}
	else
	{
		DashSystemComponent->IsDashSaved = true;
	}
}

//------------------------------------------------------ Light Attack Actions -----------------------------------------------------------------//


void APlayableCharacter::InputLightAttack()
{
	GroundedComboStringComponent->IsHeavyAttackSaved = false;

	if (DashSystemComponent->CanDashAttack == true)
	{
		DashSystemComponent->CanDashAttack = false;
		PlayAnimMontage(ForwardLightDashAttack);
	}

	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Dash };
	if (IsStateEqualToAny(MakeArray))
	{
		GroundedComboStringComponent->IsLightAttackSaved = true;
	}
	else
	{
		GroundedComboStringComponent->AppendLightAttack();
	}
}

//--------------------------------------------------------- Heavy Attack Actions -----------------------------------------------------------------//


void APlayableCharacter::InputHeavyAttack()
{
	GroundedComboStringComponent->IsLightAttackSaved = false;

	if (DashSystemComponent->CanDashAttack == true)
	{
		ResetComboString();
		DashSystemComponent->CanDashAttack = false;
		PlayAnimMontage(ForwardHeavyDashAttack);
	}
	

	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Dash };
	if (IsStateEqualToAny(MakeArray))
	{
		GroundedComboStringComponent->IsHeavyAttackSaved = true;
	}
	else
	{
		GroundedComboStringComponent->AppendHeavyAttack();
	}
}

//------------------------------------------------------ Pause Attacks -----------------------------------------------------------------//

void APlayableCharacter::StartSprintTimer()
{
	IsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
}

void APlayableCharacter::ClearSprintTimer()
{
	
	IsSprinting = false;
	IsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
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
void APlayableCharacter::SpawnLeftWeapon()
{
	if (MeleeWeapon_L)
	{
		MeleeWeapon_L->Destroy();
	}

	if (MeleeWeapon_L = GetWorld()->SpawnActor<AMeleeWeapon>(HeavyMeleeWeaponClass, GetMesh()->GetSocketTransform(TEXT("hand_l_player_weapon_socket"))))
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
		MeleeWeapon_L->AttachToComponent(GetMesh(), AttachmentRules, TEXT("hand_l_player_weapon_socket"));
	}
}

void APlayableCharacter::DestroyLeftWeapon()
{
	if (!MeleeWeapon_L)
	{
		return;
	}

	MeleeWeapon_L->Destroy();
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


