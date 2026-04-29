#include "FTAAbilitySystem/AbilityTasks/FTAAT_MoveToLocationAndWait.h"

#include "AIController.h"
#include "CombatComponents/AerialCombatComponent.h"
#include "DataAsset/MoveToLocationDataAsset.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UFTAAT_MoveToLocationAndWait* UFTAAT_MoveToLocationAndWait::FTAAT_MoveToLocationAndWait(UGameplayAbility* OwningAbility, TObjectPtr<UMoveToLocationDataAsset> MoveToLocationData)
{
	UFTAAT_MoveToLocationAndWait* Task = NewAbilityTask<UFTAAT_MoveToLocationAndWait>(OwningAbility);
	Task->MoveToLocationData = MoveToLocationData;
	return Task;
}

UFTAAT_MoveToLocationAndWait::UFTAAT_MoveToLocationAndWait(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

void UFTAAT_MoveToLocationAndWait::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if(IsMoving)
	{
		UpdateLocation(DeltaTime);
	}
}

void UFTAAT_MoveToLocationAndWait::Activate()
{
	Super::Activate();

	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
	FTAChar = Cast<AFTACharacter>(Character);

	if(!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAT_LaunchCharacterAndWait::Activate - Character is Null"));
		EndTask();
		return;
	}
	
	if(!FTAChar)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAT_LaunchCharacterAndWait::Activate - FTAChar is Null"));
		EndTask();
		return;
	}

	CMC = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());

	if(!CMC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAT_LaunchCharacterAndWait::Activate - CMC is Null"));
		EndTask();
		return;
	}

	IsMoving = true;

	ElapsedTime = 0.0f;
	StartTime = GetWorld()->GetTimeSeconds();
	
	StartLocation = GetAvatarActor()->GetActorLocation();

	FVector TargetEndLocation;
	if(!MoveToLocationData->LocationData.EndLocationVector.IsNearlyZero())
	{
		TargetEndLocation = MoveToLocationData->LocationData.EndLocationVector;
		UE_LOG(LogTemp, Warning, TEXT("HERE111"));
		TargetEndLocation.X += MoveToLocationData->LocationData.RelativeOffsetVector.X;
		TargetEndLocation.Y += MoveToLocationData->LocationData.RelativeOffsetVector.Y;
		TargetEndLocation.Z += MoveToLocationData->LocationData.RelativeOffsetVector.Z;
		
		DrawDebugSphere(
				GetWorld(),
				TargetEndLocation,
				25.0f,        
				12,           
				FColor::Red,
				false,      
				2.0f          
				);
	}
	else
	{
		TargetEndLocation = GetAvatarActor()->GetActorLocation()
		+ GetAvatarActor()->GetActorForwardVector() * MoveToLocationData->LocationData.LocationOffset.X
		+ GetAvatarActor()->GetActorRightVector()   * MoveToLocationData->LocationData.LocationOffset.Y
		+ GetAvatarActor()->GetActorUpVector()      * MoveToLocationData->LocationData.LocationOffset.Z;
		UE_LOG(LogTemp, Warning, TEXT("HERE2222"));
		
	}
	
	FHitResult Hit;

	float Radius = 34.f;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetAvatarActor());

	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		StartLocation,
		TargetEndLocation,
		FQuat::Identity,
		ECC_Visibility,
		Sphere,
		Params
	);
	
	DrawDebugLine(GetWorld(), StartLocation, TargetEndLocation, FColor::Green, false, 2.f);
	
	if (bHit)
	{
		EndLocation = Hit.Location;
	}
	else
	{
		EndLocation = TargetEndLocation;
	}

	// DrawDebugSphere(
	// GetWorld(),
	// EndLocation,
	// 25.0f,        
	// 12,           
	// FColor::Green,
	// false,      
	// 2.0f          
	// );

	StartLocation = GetAvatarActor()->GetActorLocation();

	float OriginalDistance = MoveToLocationData->LocationData.LocationOffset.Size();
	float ActualDistance = FVector::Dist(StartLocation, EndLocation);

	if (OriginalDistance > KINDA_SMALL_NUMBER)
	{
		AdjustedDuration = MoveToLocationData->LocationData.Duration * (ActualDistance / OriginalDistance);
	}
	else
	{
		AdjustedDuration = MoveToLocationData->LocationData.Duration;
	}
}

void UFTAAT_MoveToLocationAndWait::ExternalCancel()
{
	Super::ExternalCancel();
}

FString UFTAAT_MoveToLocationAndWait::GetDebugString() const
{
	return Super::GetDebugString();
}

void UFTAAT_MoveToLocationAndWait::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);

	MoveToLocationData->LocationData.EndLocationVector = FVector::ZeroVector;
	MoveToLocationData->LocationData.RelativeOffsetVector = FVector::ZeroVector;
}
void UFTAAT_MoveToLocationAndWait::UpdateLocation(float DeltaTime)
{
	ElapsedTime += DeltaTime;

	const float Alpha = FMath::Clamp(ElapsedTime / AdjustedDuration, 0.0f, 1.0f);
	const FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);

	FHitResult Hit;
	GetAvatarActor()->SetActorLocation(NewLocation, true, &Hit);

	const float DistanceToTarget = FVector::Dist(NewLocation, EndLocation);

	if (Alpha >= 1.0f || Hit.bBlockingHit || DistanceToTarget <= 10.0f)
	{
		LocationReached();
	}
}

void UFTAAT_MoveToLocationAndWait::LocationReached()
{
	CMC->Velocity.Z = 0.0f;
	CMC->GravityScale = 1.0f;

	if(MoveToLocationData->EnableAerialCombat)
	{
		UAerialCombatComponent* ACC = FTAChar->FindComponentByClass<UAerialCombatComponent>();
		if(ACC)
		{
			ACC->EnableComponent(MoveToLocationData->PostMovementMode);
		}
	}
	
	OnMoveCompleted.Broadcast();
	EndTask();
}
