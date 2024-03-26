#include "MovementComponents/PositionalWarpingComponent.h"
#include "Components/ArrowComponent.h"
#include "Enums/EFacingDirection.h"
#include "MotionWarpingComponent.h"
#include "Characters/FTACharacter.h"

UPositionalWarpingComponent::UPositionalWarpingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPositionalWarpingComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeArrowNeighbors();

}

void UPositionalWarpingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPositionalWarpingComponent::InitializeArrowNeighbors()
{
	AFTACharacter* OwningCharacter = Cast<AFTACharacter>(GetOwner());

	ArrowNeighborMap.Add(OwningCharacter->FrontArrow, ArrowNeighbors{ OwningCharacter->LeftArrow, OwningCharacter->RightArrow });
	ArrowNeighborMap.Add(OwningCharacter->BackArrow, ArrowNeighbors{ OwningCharacter->RightArrow, OwningCharacter->LeftArrow });
	ArrowNeighborMap.Add(OwningCharacter->LeftArrow, ArrowNeighbors{ OwningCharacter->BackArrow, OwningCharacter->FrontArrow });
	ArrowNeighborMap.Add(OwningCharacter->RightArrow, ArrowNeighbors{ OwningCharacter->FrontArrow, OwningCharacter->BackArrow });
	ArrowNeighborMap.Add(OwningCharacter->FrontLeftArrow, ArrowNeighbors{ OwningCharacter->BackLeftArrow, OwningCharacter->FrontRightArrow });
	ArrowNeighborMap.Add(OwningCharacter->FrontRightArrow, ArrowNeighbors{ OwningCharacter->FrontLeftArrow, OwningCharacter->BackRightArrow });
	ArrowNeighborMap.Add(OwningCharacter->BackLeftArrow, ArrowNeighbors{ OwningCharacter->BackRightArrow, OwningCharacter->FrontLeftArrow });
	ArrowNeighborMap.Add(OwningCharacter->BackRightArrow, ArrowNeighbors{ OwningCharacter->FrontRightArrow, OwningCharacter->BackLeftArrow });
}

TObjectPtr<UArrowComponent> UPositionalWarpingComponent::GetLeftArrowNeighbor(TObjectPtr<UArrowComponent> Arrow)
{
	if (ArrowNeighborMap.Contains(Arrow))
	{
		TObjectPtr<UArrowComponent> LeftNeighbor = ArrowNeighborMap[Arrow].LeftNeighbor;
		if (LeftNeighbor)
		{
			return LeftNeighbor;
		}
	}
	return nullptr;
}

TObjectPtr<UArrowComponent> UPositionalWarpingComponent::GetRightArrowNeighbor(TObjectPtr<UArrowComponent> Arrow)
{
	if (ArrowNeighborMap.Contains(Arrow))
	{
		TObjectPtr<UArrowComponent> RightNeighbor = ArrowNeighborMap[Arrow].RightNeighbor;
		if (RightNeighbor)
		{
			return RightNeighbor;
		}
	}
	return nullptr;
}

void UPositionalWarpingComponent::UpdateWarpTargetPostion(FMotionWarpingTarget MotionWarpingTargetParams)
{
	UMotionWarpingComponent* MotionWarpingComponent = GetOwner()->FindComponentByClass<UMotionWarpingComponent>();
	if (MotionWarpingComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warping Target: %s"), *MotionWarpingTargetParams.BoneName.ToString());
		MotionWarpingComponent->AddOrUpdateWarpTarget(MotionWarpingTargetParams);
	}
}

void UPositionalWarpingComponent::ResetWarpTargetPostion(FName TargetName)
{
	UMotionWarpingComponent* MotionWarpingComponent = GetOwner()->FindComponentByClass<UMotionWarpingComponent>();
	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveWarpTarget(TargetName);
		CurrentWarpPositionalArrow = NULL;
	}
}

TObjectPtr<UArrowComponent> UPositionalWarpingComponent::GetPositionalArrow(EFacingDirection FacingDirection)
{
	AFTACharacter* OwningCharacter = Cast<AFTACharacter>(GetOwner());
	switch (FacingDirection)
	{
	case EFacingDirection::EFacingDirection_Left:
		return OwningCharacter->LeftArrow;

	case EFacingDirection::EFacingDirection_Right:
		return OwningCharacter->RightArrow;
	
	case EFacingDirection::EFacingDirection_Front:
		return OwningCharacter->FrontArrow;

	case EFacingDirection::EFacingDirection_Back:
		return OwningCharacter->BackArrow;

	case EFacingDirection::EFacingDirection_FrontLeft:
		return OwningCharacter->FrontLeftArrow;

	case EFacingDirection::EFacingDirection_FrontRight:
		return OwningCharacter->FrontRightArrow;

	case EFacingDirection::EFacingDirection_BackLeft:
		return OwningCharacter->BackLeftArrow;

	case EFacingDirection::EFacingDirection_BackRight:
		return OwningCharacter->BackRightArrow;

	default:
		return OwningCharacter->FrontArrow;
	}
}

EFacingDirection UPositionalWarpingComponent::GetFacingDirection(FVector FacingLocation)
{
	AFTACharacter* OwningCharacter = Cast<AFTACharacter>(GetOwner());
	TArray<float> DistanceArray;
	float ClosestArrowDistance = 1000.0f;
	int ClosestArrowIndex = 0;

	DistanceArray.Add(FVector::Dist(FacingLocation, OwningCharacter->LeftArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, OwningCharacter->RightArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, OwningCharacter->FrontArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, OwningCharacter->BackArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, OwningCharacter->FrontLeftArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, OwningCharacter->FrontRightArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, OwningCharacter->BackLeftArrow->GetComponentLocation()));
	DistanceArray.Add(FVector::Dist(FacingLocation, OwningCharacter->BackRightArrow->GetComponentLocation()));

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
