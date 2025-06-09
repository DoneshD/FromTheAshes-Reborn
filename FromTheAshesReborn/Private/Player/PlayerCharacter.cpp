#include "Player/PlayerCharacter.h"
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
	SpringArmAttachmentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SprintArmAttachmentMesh"));
	SpringArmAttachmentMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(SpringArmAttachmentMesh);

	SpringArmComp->TargetArmLength = 400.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	SpringArmComp->SetUsingAbsoluteRotation(true);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

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

	// TargetSystemComponent = CreateDefaultSubobject<UTargetSystemComponent>("TargetSystemComponent");

	TargetingSystemComponent = CreateDefaultSubobject<UTargetingSystemComponent>("TargetingSystemComponent");

	ParkourSystemComponent = CreateDefaultSubobject<UParkourSystemComponent>(TEXT("ParkourSystemComponent"));
	this->AddOwnedComponent(ParkourSystemComponent);

}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!ParkourSystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] APlayerCharacter::BeginPlay - ParkourSystemComponent is null"), *GetActorNameOrLabel());
		return;
	}

	ParkourSystemComponent->SetIntializeReference(this, SpringArmComp, CameraComp, MotionWarpingComponent);

	if (SpringArmAttachmentMesh)
	{
		InitialSpringMeshLocation = SpringArmAttachmentMesh->GetRelativeLocation();
	}
	
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
