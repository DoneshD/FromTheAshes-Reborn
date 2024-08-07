#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Player/FTAPlayerState.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"

APlayerCharacter::APlayerCharacter(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
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

	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.7f;
	GetCharacterMovement()->AirControlBoostMultiplier = 6.f;
	
	//Walk and crouch
	GetCharacterMovement()->SetCrouchedHalfHeight(48.f);
	//GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void APlayerCharacter::InitAbilitySystemComponent()
{
	AFTAPlayerState* FTAPlayerState = GetPlayerState<AFTAPlayerState>();
	
	if(!FTAPlayerState)
	{
		return;
	}
	
	AbilitySystemComponent = CastChecked<UFTAAbilitySystemComponent>(FTAPlayerState->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(FTAPlayerState, this);
	AttributeSet = FTAPlayerState->GetAttributeSet();
}

void APlayerCharacter::FinishDying()
{
	Super::FinishDying();
}

USkeletalMeshComponent* APlayerCharacter::GetSkeletalMesh() const
{
	return GetMesh();
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void APlayerCharacter::LookUp(float Value)
{
}

void APlayerCharacter::LookUpRate(float Value)
{
}

void APlayerCharacter::Turn(float Value)
{
}

void APlayerCharacter::TurnRate(float Value)
{
}

void APlayerCharacter::MoveForward(float Value)
{
}

void APlayerCharacter::MoveRight(float Value)
{
}

void APlayerCharacter::InitializeFloatingStatusBar()
{
}

void APlayerCharacter::BindASCInput()
{
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	//TODO: Not init correctly
	Super::PossessedBy(NewController);
	InitAbilitySystemComponent();
	//only grants on server, change later
	AddDefaultAbilities();
	InitializeAttributes();
}
