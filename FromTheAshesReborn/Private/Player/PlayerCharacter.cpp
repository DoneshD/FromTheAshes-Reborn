#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Player/FTAPlayerState.h"
#include "FTAAbilitySystem/FTAAbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerCharacter::APlayerCharacter()
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
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
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

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitAbilitySystemComponent();
	//only grants on server, change later
	GiveDefaultAbilities();
	InitDefaultAttributes();
	
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	InitAbilitySystemComponent();
	InitDefaultAttributes();
}

void APlayerCharacter::AddCharacterAbilities()
{

	// for (TSubclassOf<UFTAGameplayAbility>& StartupAbility : DefaultAbilites)
	// {
	// 	AbilitySystemComponent->GiveAbility(
	// 		FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	// }
	//

	//AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}
