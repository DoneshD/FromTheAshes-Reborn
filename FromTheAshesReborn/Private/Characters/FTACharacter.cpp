#include "Characters/FTACharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DamageSystem/DamageSystem.h"
#include "MotionWarpingComponent.h"
#include "Components/ArrowComponent.h"
#include "CombatComponents/AttackTokenSystemComponent.h"
#include "MovementComponents/PositionalWarpingComponent.h"
#include "Enums/EFacingDirection.h"
#include "Interfaces/PositionalWarpingInterface.h"
#include "MovementComponents/CustomCharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FTACharacter)

AFTACharacter::AFTACharacter(const FObjectInitializer& object_initializer)
	: Super(object_initializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 1700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	RootComponent = GetCapsuleComponent();

	FrontArrow = CreateDefaultSubobject<UArrowComponent>("FrontArrowComp");
	FrontArrow->SetupAttachment(RootComponent);

	BackArrow = CreateDefaultSubobject<UArrowComponent>("BackArrowComp");
	BackArrow->SetupAttachment(RootComponent);

	LeftArrow = CreateDefaultSubobject<UArrowComponent>("LeftArrowComp");
	LeftArrow->SetupAttachment(RootComponent);

	RightArrow = CreateDefaultSubobject<UArrowComponent>("RightArrowComp");
	RightArrow->SetupAttachment(RootComponent);

	FrontLeftArrow = CreateDefaultSubobject<UArrowComponent>("FrontLeftArrowComp");
	FrontLeftArrow->SetupAttachment(RootComponent);

	BackLeftArrow = CreateDefaultSubobject<UArrowComponent>("BackLeftArrowComp");
	BackLeftArrow->SetupAttachment(RootComponent);

	FrontRightArrow = CreateDefaultSubobject<UArrowComponent>("FrontRightArrowComp");
	FrontRightArrow->SetupAttachment(RootComponent);

	BackRightArrow = CreateDefaultSubobject<UArrowComponent>("BackRightArrowComp");
	BackRightArrow->SetupAttachment(RootComponent);

	DamageSystemComponent = CreateDefaultSubobject<UDamageSystem>(TEXT("DamageSystemComponent"));
	this->AddOwnedComponent(DamageSystemComponent);

	PositionalWarpingComponent = CreateDefaultSubobject<UPositionalWarpingComponent>(TEXT("PositionalWarpingComponent"));
	this->AddOwnedComponent(PositionalWarpingComponent);

	AttackTokenSystemComponent = CreateDefaultSubobject<UAttackTokenSystemComponent>(TEXT("AttackTokenSystemComponent"));
	this->AddOwnedComponent(AttackTokenSystemComponent);

}

void AFTACharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AFTACharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//--------------------------------------------------------- FSM -----------------------------------------------------------------//

EStates AFTACharacter::GetState() const
{
	return CurrentState;
}

void AFTACharacter::SetState(EStates NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;
	}
}

bool AFTACharacter::IsStateEqualToAny(TArray<EStates> StatesToCheck)
{
	return StatesToCheck.Contains(CurrentState);
}

void AFTACharacter::UpdateWarpTargetPostion(FMotionWarpingTarget MotionWarpingTargetParams)
{
	PositionalWarpingComponent->UpdateWarpTargetPostion(MotionWarpingTargetParams);
}

void AFTACharacter::ResetWarpTargetPostion(FName TargetName)
{
	PositionalWarpingComponent->ResetWarpTargetPostion(TargetName);
}

TObjectPtr<UArrowComponent> AFTACharacter::GetPositionalArrow(EFacingDirection FacingDirection)
{
	return PositionalWarpingComponent->GetPositionalArrow(FacingDirection);
}

TObjectPtr<UArrowComponent> AFTACharacter::GetLeftArrowNeighbor(TObjectPtr<UArrowComponent> Arrow)
{
	return PositionalWarpingComponent->GetLeftArrowNeighbor(Arrow);
}

TObjectPtr<UArrowComponent> AFTACharacter::GetRightArrowNeighbor(TObjectPtr<UArrowComponent> Arrow)
{
	return PositionalWarpingComponent->GetRightArrowNeighbor(Arrow);
}

EFacingDirection AFTACharacter::GetFacingDirection(FVector FacingLocation)
{
	return PositionalWarpingComponent->GetFacingDirection(FacingLocation);
}
