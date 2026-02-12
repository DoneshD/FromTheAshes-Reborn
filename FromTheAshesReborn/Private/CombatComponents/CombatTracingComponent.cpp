#include "CombatComponents/CombatTracingComponent.h"

#include "MotionWarpingComponent.h"
#include "CombatComponents/AfterImageComponent.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/InputReadingFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/FTAPlayerState.h"

UCombatTracingComponent::UCombatTracingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCombatTracingComponent::BeginPlay()
{
	Super::BeginPlay();

	FTACharacter = Cast<AFTACharacter>(GetOwner());

	if (!FTACharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] UMeleeWarpingComponent::BeginPlay - FTACharacter is null"), *GetOwner()->GetActorNameOrLabel());
		return;
	}

	MotionWarpingComponent= FTACharacter->FindComponentByClass<UMotionWarpingComponent>();

	if (!MotionWarpingComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] UMeleeWarpingComponent::BeginPlay - MotionWarpingComp is null"), *GetOwner()->GetActorNameOrLabel());
		return;
	}

	PlayerState = Cast<AFTAPlayerState>(FTACharacter->GetPlayerState());
	if(!PlayerState)
	{
		// UE_LOG(LogTemp, Error, TEXT("[%s] UMeleeWarpingComponent::BeginPlay - PlayerState is null"), *GetOwner()->GetActorNameOrLabel());
		return;
	}
	
}

void UCombatTracingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

AActor* UCombatTracingComponent::TraceForEnemyActor(FCombatTraceData TraceData)
{
	TArray<FHitResult> OutHits;
	FVector TraceStartLocation = GetOwner()->GetActorLocation() + GetTraceDirection() * TraceData.StartTraceLocationOffset;
	FVector TraceEndLocation =  GetOwner()->GetActorLocation() + GetTraceDirection() * TraceData.EndTraceLocationOffset;
	
	TArray<AActor*> ActorArray;
	ActorArray.Add(GetOwner());

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(FTACharacter->TargetObjectTraceChannel));
	
	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		TraceStartLocation,
		TraceEndLocation,
		TraceData.TraceRadius,
		ObjectTypes,
		false,
		ActorArray,
		EDrawDebugTrace::None,
		OutHits,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
		);

	if(bHit)
	{
		AActor* BestActor = nullptr;
		AActor* ClosestActorToOwner = FilterClosestActorToOwner(OutHits);
		AActor* ClosestActorToAxis = FilterClosestActorToAxisTrace(OutHits, TraceStartLocation, TraceEndLocation);

		if(ClosestActorToOwner == ClosestActorToAxis)
		{
			BestActor = ClosestActorToOwner;
		}
		else
		{
			float DistanceFromClosestActor = FVector::Dist(ClosestActorToOwner->GetActorLocation(), GetOwner()->GetActorLocation());
			float DistanceFromAxisActor = FVector::Dist(ClosestActorToAxis->GetActorLocation(), GetOwner()->GetActorLocation());

			float AxisDistanceForAxisActor = GetDistanceFromActorToAxis(ClosestActorToAxis, TraceStartLocation, TraceEndLocation);
			float AxisDistanceForClosestActor = GetDistanceFromActorToAxis(ClosestActorToOwner, TraceStartLocation, TraceEndLocation);

			float DifferenceRawDistance = FMath::Abs(DistanceFromClosestActor - DistanceFromAxisActor);
			float DifferenceAxisDistance = FMath::Abs(AxisDistanceForAxisActor - AxisDistanceForClosestActor);

			/*
			UE_LOG(LogTemp, Warning, TEXT("Closest Actor Raw Distance: %f"), DistanceFromClosestActor);
			UE_LOG(LogTemp, Warning, TEXT("Axis Actor Raw Distance: %f"), DistanceFromAxisActor);

			UE_LOG(LogTemp, Warning, TEXT("Axis Actor Axis Distance: %f"), AxisDistanceForAxisActor);
			UE_LOG(LogTemp, Warning, TEXT("Closest Actor Axis Distance: %f"), AxisDistanceForClosestActor);

			UE_LOG(LogTemp, Warning, TEXT("DifferenceRawDistance: %f"), DifferenceRawDistance);
			UE_LOG(LogTemp, Warning, TEXT("DifferenceAxisDistance: %f"), DifferenceAxisDistance);
			*/
			
			
			if (DifferenceRawDistance / (DifferenceAxisDistance) < 0.33f)
			{
				BestActor =  ClosestActorToOwner;
			}
			else
			{
				BestActor = ClosestActorToAxis;
			}
		}
		return BestActor;
	}
	return nullptr;
}

void UCombatTracingComponent::RemoveEnemyActor()
{
	EnemyActor = nullptr;
}

float UCombatTracingComponent::GetDistanceFromActorToAxis(AActor* ActorToCheck, FVector TraceStartLocation, FVector TraceEndLocation)
{
	FVector TraceAxis = (TraceEndLocation - TraceStartLocation).GetSafeNormal();
	
	FVector StartToActor = ActorToCheck->GetActorLocation() - TraceStartLocation;
	float Dot = FVector::DotProduct(StartToActor, TraceAxis);
	FVector ClosestPointToAxis = TraceStartLocation + Dot * TraceAxis;
			
	float Distance = FVector::Dist(ActorToCheck->GetActorLocation(), ClosestPointToAxis);

	return Distance;
}

AActor* UCombatTracingComponent::FilterClosestActorToAxisTrace(TArray<FHitResult> HitArray, FVector TraceStartLocation, FVector TraceEndLocation)
{
	TSet<AActor*> UniqueHitActors;
	float DistanceToClosestActor = FLT_MAX;
	AActor* ClosestActor = nullptr;

	// FVector TraceAxis = (TraceEndLocation - TraceStartLocation).GetSafeNormal();
	
	for(FHitResult Hit : HitArray)
	{
		if(Hit.GetActor() && !UniqueHitActors.Contains(Hit.GetActor()))
		{
			UniqueHitActors.Add(Hit.GetActor());
			
			// FVector StartToActor = Hit.GetActor()->GetActorLocation() - TraceStartLocation;
			// float Dot = FVector::DotProduct(StartToActor, TraceAxis);
			// FVector ClosestPointToAxis = TraceStartLocation + Dot * TraceAxis;

			// float Distance = FVector::DistSquared(Hit.GetActor()->GetActorLocation(), ClosestPointToAxis);
			
			float Distance = GetDistanceFromActorToAxis(Hit.GetActor(), TraceStartLocation, TraceEndLocation);
			
			if(Distance < DistanceToClosestActor)
			{
				DistanceToClosestActor = Distance;
				ClosestActor = Hit.GetActor();
			}
		}
	}

	if(!ClosestActor)
	{
		return HitArray[0].GetActor();
	}
	else
	{
		return ClosestActor;
	}
}

AActor* UCombatTracingComponent::FilterClosestActorToOwner(TArray<FHitResult> HitArray)
{
	TSet<AActor*> UniqueHitActors;
	float DistanceToClosestActor = FLT_MAX;
	AActor* ClosestActor = nullptr;
	
	for(FHitResult Hit : HitArray)
	{
		if(Hit.GetActor() && !UniqueHitActors.Contains(Hit.GetActor()))
		{
			UniqueHitActors.Add(Hit.GetActor());
			float DistanceFromActor = FVector::Dist(Hit.GetActor()->GetActorLocation(), GetOwner()->GetActorLocation());
			if(DistanceFromActor < DistanceToClosestActor)
			{
				DistanceToClosestActor = DistanceFromActor;
				ClosestActor = Hit.GetActor();
			}
		}
	}

	if(!ClosestActor)
	{
		return HitArray[0].GetActor();
	}
	
	return ClosestActor;
}

FVector UCombatTracingComponent::GetTraceDirection()
{
	FVector TraceDirection;

	//TODO: Error here, fix later
	if(PlayerState)
	{
		if(!PlayerState->HardLockedTargetActor)
		{
			TraceDirection = UInputReadingFunctionLibrary::CheckInputVector(FTACharacter->GetCharacterMovement());
		}
		else
		{
			TraceDirection = (PlayerState->HardLockedTargetActor->GetActorLocation() - FTACharacter->GetActorLocation()).GetSafeNormal();
		}

		return TraceDirection;
	}
	return FVector::ZeroVector;
}