#include "Movement/GroundedParkourComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


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
	if(ParkourMasterTracerVault())
	{
		
	}
}

bool UGroundedParkourComponent::ParkourMasterTracerVault()
{
	if(!CharacterMovementComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterMovementComponent is NULL"));
		return false;

	}
	if(CharacterMovementComponent->IsFalling())
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is falling"));
		return false;
	}
	FHitResult HitResult;
	GetForwardDistanceTrace(HitResult);
	return false;
}


bool UGroundedParkourComponent::GetForwardDistanceTrace(FHitResult& OutHitResult)
{
	FVector StartLocation = OwnerCharacter->GetActorLocation();
	FVector EndLocation = (OwnerCharacter->GetActorForwardVector() * HorizontalTraceDistanceVault) + StartLocation;

	FCollisionObjectQueryParams CollisionParams;
	CollisionParams.AddObjectTypesToQuery(ECC_WorldStatic);

	GetWorld()->LineTraceSingleByObjectType(OutHitResult, StartLocation, EndLocation, CollisionParams);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 2.0f);
	return false;
}

