#include "FTAAbilitySystem/AbilityTasks/FTAAT_MoveToLocationAndWait.h"

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
	
	EndLocation = GetAvatarActor()->GetActorLocation()
	+ GetAvatarActor()->GetActorForwardVector() * MoveToLocationData->LocationOffset.X
	+ GetAvatarActor()->GetActorRightVector()   * MoveToLocationData->LocationOffset.Y
	+ GetAvatarActor()->GetActorUpVector()      * MoveToLocationData->LocationOffset.Z;

	DrawDebugSphere(
	GetWorld(),
	EndLocation,
	25.0f,        
	12,           
	FColor::Green,
	false,      
	2.0f          
);
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
}

void UFTAAT_MoveToLocationAndWait::UpdateLocation(float DeltaTime)
{
	ElapsedTime += DeltaTime;

	const float Alpha = FMath::Clamp(ElapsedTime / MoveToLocationData->Duration, 0.0f, 1.0f);
	const FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);

	FHitResult Hit;
	GetAvatarActor()->SetActorLocation(NewLocation, true, &Hit);

	if (Alpha >= 1.0f || Hit.bBlockingHit)
	{
		LocationReached();
	}
}

void UFTAAT_MoveToLocationAndWait::LocationReached()
{
	CMC->Velocity.Z = 0.0f;
	CMC->GravityScale = 1.0f;

	FVector FinalLaunchVelocity = GetAvatarActor()->GetActorForwardVector() * MoveToLocationData->LaunchVelocity.X
	+ GetAvatarActor()->GetActorRightVector()   * MoveToLocationData->LaunchVelocity.Y
	+ GetAvatarActor()->GetActorUpVector()      * MoveToLocationData->LaunchVelocity.Z;
	
	// CMC->Velocity = FVector::ZeroVector;
	CMC->Launch(FinalLaunchVelocity);

	if(MoveToLocationData->EnableAerialCombat)
	{
		UAerialCombatComponent* ACC = FTAChar->FindComponentByClass<UAerialCombatComponent>();
		if(ACC)
		{
			
		}
	}

	
	
	// if(MoveToLocationData->SetGravity)
	// {
	// 	CMC->GravityScale = MoveToLocationData->Gravity;
	// }
	// else
	// {
	// 	CMC->GravityScale = 0.0f;
	// }
	//
	// if(MoveToLocationData->SetVelocity)
	// {
	// 	CMC->Velocity = MoveToLocationData->LocationReachedVelocity;
	// }
	// else
	// {
	// 	// CMC->Velocity.Z = 0.0f;
	// 	// CMC->Velocity = FVector::ZeroVector;
	// }
	
	OnMoveCompleted.Broadcast();
	EndTask();
}
