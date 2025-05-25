#include "FTAAbilitySystem/AbilityTasks/AT_SuspendInAirAndWait.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAT_SuspendInAirAndWait* UAT_SuspendInAirAndWait::AT_SuspendInAirAndWait(UGameplayAbility* OwningAbility, float Speed, float Duration)
{
	UAT_SuspendInAirAndWait* Task = NewAbilityTask<UAT_SuspendInAirAndWait>(OwningAbility);
	Task->DescentSpeed = Speed;
	Task->SuspendDuration = Duration;
	return Task;
}

UAT_SuspendInAirAndWait::UAT_SuspendInAirAndWait(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

void UAT_SuspendInAirAndWait::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if(bDescend)
	{
		UpdateDescentMovement(DeltaTime);
	}
}

void UAT_SuspendInAirAndWait::Activate()
{
	Super::Activate();

	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());

	if(!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAT_LaunchCharacterAndWait::Activate - Character is Null"));
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

	CMC->Velocity.Z = 0.0f;
	CMC->GravityScale = 0.0f;
	
	bDescend = true;
}

void UAT_SuspendInAirAndWait::UpdateDescentMovement(float DeltaTime)
{
	FHitResult Hit;
	
	FVector CurrentLocation = GetAvatarActor()->GetActorLocation();
	
	FVector MoveDelta = FVector(0.0f, 0.0f, -1.0f).GetSafeNormal() * (DescentSpeed * FMath::Square(2)) * DeltaTime;
	// FVector MoveDelta = FVector(0.0f, 0.0f, -1.0f).GetSafeNormal() * (DescentSpeed * FMath::Square(ComboManagerComponent->AerialAttacksCounter)) * DeltaTime;
	
	FVector NewLocation = CurrentLocation + MoveDelta;

	GetAvatarActor()->SetActorLocation(NewLocation, true, &Hit);

	if (Hit.bBlockingHit)
	{
		EndStall();
	}
}

void UAT_SuspendInAirAndWait::EndStall()
{
	CMC->Velocity.Z = -100.0f;
	CMC->GravityScale = 4.0f;

	OnSuspendComplete.Broadcast();
	EndTask();
}

void UAT_SuspendInAirAndWait::ExternalCancel()
{
	Super::ExternalCancel();

}

FString UAT_SuspendInAirAndWait::GetDebugString() const
{
	return Super::GetDebugString();
}

void UAT_SuspendInAirAndWait::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);
	
	CMC->Velocity.Z = -100.0f;
	CMC->GravityScale = 4.0f;

	OnSuspendComplete.Broadcast();
}
