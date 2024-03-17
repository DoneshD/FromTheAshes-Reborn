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
#include "EFacingDirection.h"

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

void AFTACharacter::UpdateWarpTargetPostion(FMotionWarpingTarget& MotionWarpingTargetParams)
{
}

void AFTACharacter::ResetWarpTargetPostion()
{
}

TObjectPtr<UArrowComponent> AFTACharacter::GetPositionalArrow(EFacingDirection FacingDirection)
{
	switch (FacingDirection)
	{
	case EFacingDirection::EFacingDirection_Left:
		return LeftArrow;

	case EFacingDirection::EFacingDirection_Right:
		return RightArrow;

	case EFacingDirection::EFacingDirection_Front:
		return FrontArrow;

	case EFacingDirection::EFacingDirection_Back:
		return BackArrow;

	case EFacingDirection::EFacingDirection_FrontLeft:
		return FrontLeftArrow;

	case EFacingDirection::EFacingDirection_FrontRight:
		return FrontRightArrow;

	case EFacingDirection::EFacingDirection_BackLeft:
		return BackLeftArrow;

	case EFacingDirection::EFacingDirection_BackRight:
		return BackRightArrow;

	default:
		return FrontArrow;
	}
}

EFacingDirection AFTACharacter::GetFacingDirection(FVector FacingLocation)
{
	TArray<float> DistanceArray;
	float ClosestArrowDistance = 1000.0f;
	int ClosestArrowIndex = 0;

	DistanceArray.Add(FVector::Dist(FacingLocation, LeftArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, RightArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, FrontArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, BackArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, FrontLeftArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, FrontRightArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, BackLeftArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, BackRightArrow->GetComponentLocation()));

	for (const float& EachArrowDistance : DistanceArray)
	{
		if (EachArrowDistance < ClosestArrowDistance)
		{
			ClosestArrowDistance = EachArrowDistance;
			ClosestArrowIndex = DistanceArray.Find(EachArrowDistance);
		}
	}
	switch (ClosestArrowIndex)
	{
	case 0:
		return EFacingDirection::EFacingDirection_Left;

	case 1:
		return EFacingDirection::EFacingDirection_Right;

	case 2:
		return EFacingDirection::EFacingDirection_Front;

	case 3:
		return EFacingDirection::EFacingDirection_Back;

	case 4:
		return EFacingDirection::EFacingDirection_FrontLeft;

	case 5:
		return EFacingDirection::EFacingDirection_FrontRight;

	case 6:
		return EFacingDirection::EFacingDirection_BackLeft;

	case 7:
		return EFacingDirection::EFacingDirection_BackRight;

	default:
		break;
	}
	return EFacingDirection::EFacingDirection_None;
}
