#include "Movement/GroundedParkourComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


UGroundedParkourComponent::UGroundedParkourComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UGroundedParkourComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());

	if(!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerCharacter is NULL"));
		return;
	}
	
	CharacterMovementComponent = Cast<UCharacterMovementComponent>(OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>());
	
	if(!CharacterMovementComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterMovementComponent is NULL"));
		return;
	}

	CapsuleRadius = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	CapsuleHalfHeight = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	JumpZVelocity = CharacterMovementComponent->JumpZVelocity;

}


void UGroundedParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UGroundedParkourComponent::ParkourInputPressedVault()
{
	
}


