#include "CombatComponents/MeleeWarpingComponent.h"

#include "MotionWarpingComponent.h"
#include "CombatComponents/AfterImageComponent.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/InputReadingFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/FTAPlayerState.h"

UMeleeWarpingComponent::UMeleeWarpingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UMeleeWarpingComponent::BeginPlay()
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

void UMeleeWarpingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UMeleeWarpingComponent::UpdateWarpTarget(FVector TargetLocation, FRotator TargetRotation)
{
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(CurrentWarpTargetName, TargetLocation,  FRotator(0, TargetRotation.Yaw, 0));
}

float UMeleeWarpingComponent::GetDistanceFromActorToAxis(AActor* ActorToCheck, FVector TraceStartLocation, FVector TraceEndLocation)
{
	FVector TraceAxis = (TraceEndLocation - TraceStartLocation).GetSafeNormal();
	
	FVector StartToActor = ActorToCheck->GetActorLocation() - TraceStartLocation;
	float Dot = FVector::DotProduct(StartToActor, TraceAxis);
	FVector ClosestPointToAxis = TraceStartLocation + Dot * TraceAxis;
			
	float Distance = FVector::Dist(ActorToCheck->GetActorLocation(), ClosestPointToAxis);

	return Distance;
}

AActor* UMeleeWarpingComponent::FilterClosestActorToAxisTrace(TArray<FHitResult> HitArray, FVector TraceStartLocation, FVector TraceEndLocation)
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

AActor* UMeleeWarpingComponent::FilterClosestActorToOwner(TArray<FHitResult> HitArray)
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

void UMeleeWarpingComponent::RemoveWarpTarget()
{
	MotionWarpingComponent->RemoveWarpTarget(CurrentWarpTargetName);
}

void UMeleeWarpingComponent::WarpToTarget(FMeleeWarpData WarpData, bool InvertLocation, bool TestHitWarp)
{
	CurrentWarpTargetName = WarpData.WarpTargetName;

	if(TestHitWarp)
	{
		FVector TestLocation;

		TestLocation = GetOwner()->GetActorLocation() + (GetOwner()->GetActorForwardVector() * -WarpData.StartTraceLocationOffset);

		DrawDebugSphere(
			GetWorld(),
			TestLocation,
			25.f,       
			12,          
			FColor::Red, 
			false,       
			2.f          
		);
		
		UpdateWarpTarget(TestLocation, GetOwner()->GetActorRotation());
		return;
	}
	
	TArray<FHitResult> OutHits;
	FVector TraceStartLocation = GetOwner()->GetActorLocation() + GetTraceDirection() * WarpData.StartTraceLocationOffset;
	FVector TraceEndLocation =  GetOwner()->GetActorLocation() + GetTraceDirection() * WarpData.EndTraceLocationOffset;
	
	TArray<AActor*> ActorArray;
	ActorArray.Add(GetOwner());

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(FTACharacter->TargetObjectTraceChannel));
	
	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		TraceStartLocation,
		TraceEndLocation,
		WarpData.TraceRadius,
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
			FVector EnemyLocation = EnemyActor->GetActorLocation();
			FVector OffsetDirection = (GetOwner()->GetActorLocation() - EnemyActor->GetActorLocation()).GetSafeNormal();
			
			if(InvertLocation)
			{
				OffsetDirection = OffsetDirection * (-1.0f);
			}
			
			OffsetDirection.Z = 0.0f;
			
			
			FVector WarpTargetLocation = EnemyLocation + OffsetDirection * WarpData.WarpTargetLocationOffset;
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

			// DrawDebugSphere(GetWorld(), ClosestActorToOwner->GetActorLocation(), 25.0f, 12, FColor::Blue, false, 5.0f);
			// DrawDebugSphere(GetWorld(), ClosestActorToAxis->GetActorLocation(), 25.0f, 12, FColor::Red, false, 5.0f);


			UAfterImageComponent* AIC = FTACharacter->FindComponentByClass<UAfterImageComponent>();
			
			if(GetOwner()->GetDistanceTo(EnemyActor) >= 350.0f)
			{
				AIC->CreateAfterImage();
			}
		
			UpdateWarpTarget(WarpTargetLocation, WarpTargetRotation);
		}
	}
	
}

FVector UMeleeWarpingComponent::GetTraceDirection()
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
	



