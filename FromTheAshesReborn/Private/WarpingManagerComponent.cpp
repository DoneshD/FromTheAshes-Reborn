#include "WarpingManagerComponent.h"

#include "MotionWarpingComponent.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/InputReadingFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/FTAPlayerState.h"

UWarpingManagerComponent::UWarpingManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UWarpingManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	FTACharacter = Cast<AFTACharacter>(GetOwner());

	if (!FTACharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] UWarpingManagerComponent::BeginPlay - FTACharacter is null"), *GetOwner()->GetActorNameOrLabel());
		return;
	}

	MotionWarpingComponent= FTACharacter->FindComponentByClass<UMotionWarpingComponent>();

	if (!MotionWarpingComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] UWarpingManagerComponent::BeginPlay - MotionWarpingComp is null"), *GetOwner()->GetActorNameOrLabel());
		return;
	}

	//TODO: WIll support AI use later
	PlayerState = Cast<AFTAPlayerState>(FTACharacter->GetPlayerState());
	if(!PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] UWarpingManagerComponent::BeginPlay - PlayerState is null"), *GetOwner()->GetActorNameOrLabel());
		return;
	}
	
}

void UWarpingManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UWarpingManagerComponent::AddWarpTarget(FVector TargetLocation, FRotator TargetRotation)
{
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(CurrentWarpTargetName, TargetLocation,  FRotator(0, TargetRotation.Yaw, 0));
}

float UWarpingManagerComponent::GetDistanceFromActorToAxis(AActor* ActorToCheck, FVector TraceStartLocation, FVector TraceEndLocation)
{
	FVector TraceAxis = (TraceEndLocation - TraceStartLocation).GetSafeNormal();
	
	FVector StartToActor = ActorToCheck->GetActorLocation() - TraceStartLocation;
	float Dot = FVector::DotProduct(StartToActor, TraceAxis);
	FVector ClosestPointToAxis = TraceStartLocation + Dot * TraceAxis;
			
	float Distance = FVector::Dist(ActorToCheck->GetActorLocation(), ClosestPointToAxis);

	return Distance;
}

AActor* UWarpingManagerComponent::FilterClosestActorToAxisTrace(TArray<FHitResult> HitArray, FVector TraceStartLocation, FVector TraceEndLocation)
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

AActor* UWarpingManagerComponent::FilterClosestActorToOwner(TArray<FHitResult> HitArray)
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

void UWarpingManagerComponent::RemoveWarpTarget()
{
	MotionWarpingComponent->RemoveWarpTarget(CurrentWarpTargetName);
}

void UWarpingManagerComponent::TraceForTargets(FName WarpTargetName, float StartLocationOffset, float EndLocationDistance, float TraceRadius, float WarpTargetLocationOffset)
{
	CurrentWarpTargetName = WarpTargetName;
	
	TArray<FHitResult> OutHits;
	FVector TraceStartLocation = GetOwner()->GetActorLocation() + GetTraceDirection() * StartLocationOffset;
	FVector TraceEndLocation =  GetOwner()->GetActorLocation() + GetTraceDirection() * EndLocationDistance;
	
	TArray<AActor*> ActorArray;
	ActorArray.Add(GetOwner());

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(FTACharacter->TargetObjectTraceChannel));
	
	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		TraceStartLocation,
		TraceEndLocation,
		TraceRadius,
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
		
		//TODO: Change later
		AEnemyBaseCharacter* EnemyActor = Cast<AEnemyBaseCharacter>(BestActor);
		if (bHit && EnemyActor && !EnemyActor->IsDead)
		{
			FVector OffsetDirection = (GetOwner()->GetActorLocation() - EnemyActor->GetActorLocation()).GetSafeNormal();
			
			FVector WarpTargetLocation = EnemyActor->GetActorLocation() + OffsetDirection * WarpTargetLocationOffset;
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), EnemyActor->GetActorLocation());
			FRotator WarpTargetRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
		
			// DrawDebugSphere(GetWorld(), WarpTargetLocation, 25.0f, 12, FColor::Blue, false, 5.0f);
		
			// const FVector Forward = WarpTargetRotation.Vector();
			// DrawDebugLine(
			// 	GetWorld(),
			// 	WarpTargetLocation,
			// 	WarpTargetLocation + Forward * 100.0f,
			// 	FColor::Cyan,
			// 	false,
			// 	5.0f,
			// 	0,
			// 	2.0f
			// );

			DrawDebugSphere(GetWorld(), ClosestActorToOwner->GetActorLocation(), 25.0f, 12, FColor::Blue, false, 5.0f);
			DrawDebugSphere(GetWorld(), ClosestActorToAxis->GetActorLocation(), 25.0f, 12, FColor::Red, false, 5.0f);
			
		
			AddWarpTarget(WarpTargetLocation, WarpTargetRotation);
		}
	}
	
}

FVector UWarpingManagerComponent::GetTraceDirection()
{
	FVector TraceDirection;

	//TODO: Error here, fix later
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
	



