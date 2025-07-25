#include "Player/PlayerCharacter.h"

#include "Camera/CameraSystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/FTAPlayerState.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "ParkourSystem/ParkourSystemComponent.h"
#include "TargetingSystem/TargetingSystemComponent.h"


APlayerCharacter::APlayerCharacter(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	CameraAnchorComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CameraAnchorComponent"));
	CameraAnchorComponent->SetupAttachment(RootComponent);
	CameraAnchorComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	
	DefaultCameraAnchorRelativeLocation = CameraAnchorComponent->GetRelativeLocation();
	DefaultCameraAnchorRelativeRotation = CameraAnchorComponent->GetRelativeRotation();
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(CameraAnchorComponent);

	SpringArmComponent->TargetArmLength = 400.0f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetUsingAbsoluteRotation(true);

	DefaultSpringArmRelativeLocation = SpringArmComponent->GetRelativeLocation();
	DefaultSpringArmRelativeRotation = SpringArmComponent->GetRelativeRotation();
	DefaultSpringArmLength = SpringArmComponent->TargetArmLength;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	DefaultCameraComponentRelativeLocation = CameraComponent->GetRelativeLocation();
	DefaultCameraComponentRelativeRotation = CameraComponent->GetRelativeRotation();

	GetCharacterMovement()->JumpZVelocity = 1500.f;
	GetCharacterMovement()->GravityScale = 4.0f;
	GetCharacterMovement()->AirControl = 1.0f;
	GetCharacterMovement()->AirControlBoostMultiplier = 1.0f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 100.0f;
	
	GetCharacterMovement()->BrakingFrictionFactor = 2.0f;
	GetCharacterMovement()->BrakingFriction = 1.0f;
	GetCharacterMovement()->bUseSeparateBrakingFriction = true;
	GetCharacterMovement()->FallingLateralFriction = 50.0f;
	
	GetCharacterMovement()->SetCrouchedHalfHeight(48.f);
	// GetCharacterMovement()->MaxWalkSpeed = 100.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	
	TargetSystemComponent = CreateDefaultSubobject<UTargetingSystemComponent>(TEXT("TargetingSystemComponent"));
	ParkourSystemComponent = CreateDefaultSubobject<UParkourSystemComponent>(TEXT("ParkourSystemComponent"));
	CameraManagerComponent = CreateDefaultSubobject<UCameraSystemComponent>(TEXT("CameraSystemComponent"));
	
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!ParkourSystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] APlayerCharacter::BeginPlay - ParkourSystemComponent is null"), *GetActorNameOrLabel());
		return;
	}

	if (!TargetSystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] APlayerCharacter::BeginPlay - TargetingSystemComponent is null"), *GetActorNameOrLabel());
		return;
	}

	if (!CameraManagerComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] APlayerCharacter::BeginPlay - CameraManagerComponent is null"), *GetActorNameOrLabel());
		return;
	}

	// ParkourSystemComponent->SetIntializeReference(this, SpringArmComponent, CameraComponent, MotionWarpingComponent);

}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilitySystemComponent();
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
	
	FTAAbilitySystemComponent = CastChecked<UFTAAbilitySystemComponent>(FTAPlayerState->GetAbilitySystemComponent());
	FTAAbilitySystemComponent->InitAbilityActorInfo(FTAPlayerState, this);
	
}

USceneComponent* APlayerCharacter::GetCameraAnchorComponent()
{
	if(!CameraAnchorComponent)
	{
		return nullptr;
	}
	return CameraAnchorComponent;
}

USpringArmComponent* APlayerCharacter::GetSpringArmComponent()
{
	if(!SpringArmComponent)
	{
		return nullptr;
	}
	return SpringArmComponent;
}

UCameraComponent* APlayerCharacter::GetCameraComponentComponent()
{
	if(!CameraComponent)
	{
		return nullptr;
	}
	return CameraComponent;
}
