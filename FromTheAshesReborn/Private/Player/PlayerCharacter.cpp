#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Player/FTAPlayerState.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TargetSystemComponent.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "ParkourSystem/ParkourSystemComponent.h"


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
	// GetCharacterMovement()->MaxWalkSpeed = 100.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	TargetSystemComponent = CreateDefaultSubobject<UTargetSystemComponent>("TargetSystemComponent");

	ParkourSystemComponent = CreateDefaultSubobject<UParkourSystemComponent>(TEXT("ParkourSystemComponent"));
	this->AddOwnedComponent(ParkourSystemComponent);

}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	ParkourSystemComponent->SetIntializeReference(this, SpringArmComp, CameraComp, MotionWarpingComponent);
	
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

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilitySystemComponent();
}