// Fill out your copyright notice in the Description page of Project Settings.
#include "Characters/FTACharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DamageSystem/DamageSystem.h"
#include "MotionWarpingComponent.h"
#include "CombatComponents/AttacksComponent.h"
#include "Components/ArrowComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FTACharacter)

// Sets default values
AFTACharacter::AFTACharacter()
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

	AttacksComponent = CreateDefaultSubobject<UAttacksComponent>(TEXT("AttacksComponent"));
	this->AddOwnedComponent(AttacksComponent);

	//MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	//this->AddOwnedComponent(MotionWarpingComponent);

	//MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	//this->AddOwnedComponent(MotionWarpingComponent);

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

bool AFTACharacter::CanJump()
{
	TArray<EStates> MakeArray = { EStates::EState_Attack, EStates::EState_Dodge };
	return !IsStateEqualToAny(MakeArray);
}