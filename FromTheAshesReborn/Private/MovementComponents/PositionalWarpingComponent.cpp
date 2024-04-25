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

	TMap<float, EFacingDirection> DistanceDirectionMap;

	DistanceDirectionMap.Add(FVector::Dist(FacingLocation, OwningCharacter->LeftArrow->GetComponentLocation()), EFacingDirection::EFacingDirection_Left);
	DistanceDirectionMap.Add(FVector::Dist(FacingLocation, OwningCharacter->RightArrow->GetComponentLocation()), EFacingDirection::EFacingDirection_Right);
	DistanceDirectionMap.Add(FVector::Dist(FacingLocation, OwningCharacter->FrontArrow->GetComponentLocation()), EFacingDirection::EFacingDirection_Front);
	DistanceDirectionMap.Add(FVector::Dist(FacingLocation, OwningCharacter->BackArrow->GetComponentLocation()), EFacingDirection::EFacingDirection_Back);
	DistanceDirectionMap.Add(FVector::Dist(FacingLocation, OwningCharacter->FrontLeftArrow->GetComponentLocation()), EFacingDirection::EFacingDirection_FrontLeft);
	DistanceDirectionMap.Add(FVector::Dist(FacingLocation, OwningCharacter->FrontRightArrow->GetComponentLocation()), EFacingDirection::EFacingDirection_FrontRight);
	DistanceDirectionMap.Add(FVector::Dist(FacingLocation, OwningCharacter->BackLeftArrow->GetComponentLocation()), EFacingDirection::EFacingDirection_BackLeft);
	DistanceDirectionMap.Add(FVector::Dist(FacingLocation, OwningCharacter->BackRightArrow->GetComponentLocation()), EFacingDirection::EFacingDirection_BackRight);

	float ClosestArrowDistance = FLT_MAX;
	EFacingDirection ClosestDirection = EFacingDirection::EFacingDirection_None;
	for (const auto& Pair : DistanceDirectionMap)
	{
		if (Pair.Key < ClosestArrowDistance)
		{
			ClosestArrowDistance = Pair.Key;
			ClosestDirection = Pair.Value;
		}
	}

	FString DirectionString;
	switch (ClosestDirection)
	{
	case EFacingDirection::EFacingDirection_Left:
		break;

	case EFacingDirection::EFacingDirection_Right:
		break;

	case EFacingDirection::EFacingDirection_Front:
		break;

	case EFacingDirection::EFacingDirection_Back:
		break;

	case EFacingDirection::EFacingDirection_FrontLeft:
		break;

	case EFacingDirection::EFacingDirection_FrontRight:
		break;

	case EFacingDirection::EFacingDirection_BackLeft:
		break;

	case EFacingDirection::EFacingDirection_BackRight:
		break;

	default:
		break;
	}
	return ClosestDirection;
}
