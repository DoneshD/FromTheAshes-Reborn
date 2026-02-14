#include "Player/PlayerCharacter.h"

#include "Camera/CameraSystemComponent.h"
#include "Camera/CameraComponent.h"
#include "CombatComponents/GroupCombatComponent.h"
#include "CombatComponents/RangedCombatComponent.h"
#include "Components/WidgetComponent.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "Player/FTAPlayerState.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "ParkourSystem/ParkourSystemComponent.h"
#include "TargetingSystem/TargetingSystemComponent.h"
#include "Weapon/EquipmentManagerComponent.h"


APlayerCharacter::APlayerCharacter(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	CameraAnchorComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CameraAnchorComponent"));
	CameraAnchorComponent->SetupAttachment(RootComponent);
	CameraAnchorComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	
	DefaultCameraAnchorRelativeLocation = CameraAnchorComponent->GetRelativeLocation();
	DefaultCameraAnchorRelativeRotation = CameraAnchorComponent->GetRelativeRotation();
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(CameraAnchorComponent);

	SpringArmComponent->TargetArmLength = 400.0f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetUsingAbsoluteRotation(true);

	DefaultSpringArmRelativeLocation = SpringArmComponent->GetRelativeLocation();
	DefaultSpringArmRelativeRotation = SpringArmComponent->GetRelativeRotation();
	DefaultSpringArmLength = 250.0f;

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
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	
	TargetSystemComponent = CreateDefaultSubobject<UTargetingSystemComponent>(TEXT("TargetingSystemComponent"));
	this->AddOwnedComponent(TargetSystemComponent);
	InitializedActorComponents.AddUnique(TargetSystemComponent);
	
	ParkourSystemComponent = CreateDefaultSubobject<UParkourSystemComponent>(TEXT("ParkourSystemComponent"));
	this->AddOwnedComponent(ParkourSystemComponent);
	InitializedActorComponents.AddUnique(ParkourSystemComponent);
	
	CameraManagerComponent = CreateDefaultSubobject<UCameraSystemComponent>(TEXT("CameraSystemComponent"));
	this->AddOwnedComponent(CameraManagerComponent);
	InitializedActorComponents.AddUnique(CameraManagerComponent);
	
	RangedCombatComponent = CreateDefaultSubobject<URangedCombatComponent>(TEXT("RangedCombatComponent"));
	this->AddOwnedComponent(RangedCombatComponent);
	InitializedActorComponents.AddUnique(RangedCombatComponent);
	
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	TempEnemyCountDeath = 0;
	
	// ParkourSystemComponent->SetIntializeReference(this, SpringArmComponent, CameraComponent, MotionWarpingComponent);
	
	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
	TargetSystemComponent->OnTargetLockedOn.AddDynamic(this, &APlayerCharacter::OnTargetLockedOn);
	TargetSystemComponent->OnTargetLockedOff.AddDynamic(this, &APlayerCharacter::OnTargetLockedOff);
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// UE_LOG(LogTemp, Warning, TEXT("[%s] Movement Mode: %s"), *GetActorNameOrLabel(), *UEnum::GetValueAsString(GetCharacterMovement()->MovementMode));

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


void APlayerCharacter::OnTargetLockedOn(AActor* Target)
{
	AEnemyBaseCharacter* Enemy = Cast<AEnemyBaseCharacter>(Target);

	if(Enemy)
	{
		Enemy->HealthWidget->SetVisibility(true);
	}
}

void APlayerCharacter::OnTargetLockedOff(AActor* Target)
{
	AEnemyBaseCharacter* Enemy = Cast<AEnemyBaseCharacter>(Target);

	if(Enemy)
	{
		Enemy->HealthWidget->SetVisibility(false);
	}
}

void APlayerCharacter::SetMaxWalkSpeed(float X)
{
	GetCharacterMovement()->MaxWalkSpeed = X;
}
