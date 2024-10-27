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
	GetForwardActorTrace();
	return false;
}


bool UGroundedParkourComponent::GetForwardActorTrace()
{
    FHitResult ForwardHitResult;
    FVector StartLocation = OwnerCharacter->GetActorLocation();
    FVector EndLocation = (OwnerCharacter->GetActorForwardVector() * TraceForwardActorDistance) + StartLocation;

    FCollisionObjectQueryParams CollisionParams;
    CollisionParams.AddObjectTypesToQuery(ECC_WorldStatic);

    bool bHit = GetWorld()->LineTraceSingleByObjectType(ForwardHitResult, StartLocation, EndLocation, CollisionParams);

    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 2.0f);

    if (bHit)
    {
        DrawDebugPoint(GetWorld(), ForwardHitResult.ImpactPoint, 40.0f, FColor::Green, false, 1.0f);
    }

    TraceFirstActor = ForwardHitResult.GetActor();
    TraceFirstActorImpactPoint = ForwardHitResult.ImpactPoint;


    if (ForwardHitResult.bBlockingHit)
    {
        bool bHitVert = GetVerticalBlockerTrace();
        return bHitVert;
    }

    CanParkour = false;
    return false;

}

bool UGroundedParkourComponent::GetVerticalBlockerTrace()
{
	FHitResult VerticalHitResult;

	if (!TraceFirstActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("TraceFirstActor is NULL"));
		return false;
	}

	FVector ActorOrigin, ActorBoxExtent;
	TraceFirstActor->GetActorBounds(true, ActorOrigin, ActorBoxExtent);

	FVector ActorTop = ActorOrigin + FVector(0, 0, ActorBoxExtent.Z);

	FVector StartLocation = TraceFirstActorImpactPoint;
	StartLocation.Z = ActorTop.Z;

	FVector EndLocation = StartLocation + FVector(0, 0, CheckVerticalBlockerDistance); 

	DrawDebugPoint(GetWorld(), StartLocation, 40.0f, FColor::Purple, false, 1.0f);
	DrawDebugPoint(GetWorld(), EndLocation, 40.0f, FColor::Turquoise, false, 1.0f);

	FCollisionObjectQueryParams CollisionParams;
	CollisionParams.AddObjectTypesToQuery(ECC_WorldStatic);

	bool bHit = GetWorld()->LineTraceSingleByObjectType(VerticalHitResult, StartLocation, EndLocation, CollisionParams);

	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 2.0f);

	if (bHit)
	{
		DrawDebugPoint(GetWorld(), VerticalHitResult.ImpactPoint, 40.0f, FColor::Green, false, 1.0f);
	}

	//StartLocation.X = FMath::Max(StartLocation.X + 10.0f, ActorTop.Z); 

	EndLocation = StartLocation + FVector(CheckVerticalBlockerDistance, 0, 0); 

	//DrawDebugPoint(GetWorld(), StartLocation, 40.0f, FColor::Emerald, false, 1.0f);
	DrawDebugPoint(GetWorld(), EndLocation, 40.0f, FColor::Orange, false, 1.0f);
	

	bool bHit2 = GetWorld()->LineTraceSingleByObjectType(VerticalHitResult, StartLocation, EndLocation, CollisionParams);

	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 2.0f);

	if (bHit)
	{
		DrawDebugPoint(GetWorld(), VerticalHitResult.ImpactPoint, 40.0f, FColor::Green, false, 1.0f);
	}

	//StartLocation.Y = FMath::Max(StartLocation.Y + 10.0f, ActorTop.Z); 

	EndLocation = StartLocation + FVector(0, CheckVerticalBlockerDistance, 0); 

	//DrawDebugPoint(GetWorld(), StartLocation, 40.0f, FColor::Emerald, false, 1.0f);
	DrawDebugPoint(GetWorld(), EndLocation, 40.0f, FColor::Orange, false, 1.0f);
	

	bool bHit3 = GetWorld()->LineTraceSingleByObjectType(VerticalHitResult, StartLocation, EndLocation, CollisionParams);

	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 2.0f);

	if (bHit)
	{
		DrawDebugPoint(GetWorld(), VerticalHitResult.ImpactPoint, 40.0f, FColor::Green, false, 1.0f);
	}


	//StartLocation.Y = FMath::Max(StartLocation.Y + 10.0f, ActorTop.Z); 

	EndLocation = StartLocation + FVector(0, -CheckVerticalBlockerDistance, 0); 

	//DrawDebugPoint(GetWorld(), StartLocation, 40.0f, FColor::Emerald, false, 1.0f);
	DrawDebugPoint(GetWorld(), EndLocation, 40.0f, FColor::Orange, false, 1.0f);
	

	bool bHit4 = GetWorld()->LineTraceSingleByObjectType(VerticalHitResult, StartLocation, EndLocation, CollisionParams);

	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 2.0f);

	if (bHit)
	{
		DrawDebugPoint(GetWorld(), VerticalHitResult.ImpactPoint, 40.0f, FColor::Green, false, 1.0f);
	}


	return bHit;
}


