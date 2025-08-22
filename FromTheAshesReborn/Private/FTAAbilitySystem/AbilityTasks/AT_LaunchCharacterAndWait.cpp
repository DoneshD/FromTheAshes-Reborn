#include "FTAAbilitySystem/AbilityTasks/AT_LaunchCharacterAndWait.h"

#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

UAT_LaunchCharacterAndWait* UAT_LaunchCharacterAndWait::AT_LaunchCharacterAndWait(UGameplayAbility* OwningAbility, float VerticalDistance, float Duration, float StallTime, float Offset)
{
	UAT_LaunchCharacterAndWait* Task = NewAbilityTask<UAT_LaunchCharacterAndWait>(OwningAbility);
	Task->LaunchVerticalDistance = VerticalDistance;
	Task->LaunchDuration = Duration;
	Task->StallDuration = StallTime;
	Task->LaunchOffset = Offset;
	return Task;
}

UAT_LaunchCharacterAndWait::UAT_LaunchCharacterAndWait(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

void UAT_LaunchCharacterAndWait::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if(IsLaunching)
	{
		UpdateMovement(DeltaTime);
	}
}

void UAT_LaunchCharacterAndWait::Activate()
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


	LaunchElapsedTime = 0.0f;
	LaunchedStartTime = GetWorld()->GetTimeSeconds();
	
	LaunchStartLocation = GetAvatarActor()->GetActorLocation();
	LaunchEndLocation = GetAvatarActor()->GetActorLocation() + FVector(0.0f, 0.0f, LaunchVerticalDistance);

	IsLaunching = true;

}

void UAT_LaunchCharacterAndWait::ExternalCancel()
{
	Super::ExternalCancel();
}

FString UAT_LaunchCharacterAndWait::GetDebugString() const
{
	return Super::GetDebugString();
}

void UAT_LaunchCharacterAndWait::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);
}

void UAT_LaunchCharacterAndWait::UpdateMovement(float DeltaTime)
{
	LaunchElapsedTime += DeltaTime;

	const float Alpha = FMath::Clamp(LaunchElapsedTime / LaunchDuration, 0.0f, 1.0f);
	const FVector NewLocation = FMath::Lerp(LaunchStartLocation, LaunchEndLocation, Alpha);

	FHitResult Hit;
	GetAvatarActor()->SetActorLocation(NewLocation, true, &Hit);

	if (Alpha >= 1.0f || Hit.bBlockingHit)
	{
		LocationReached();
	}
}

void UAT_LaunchCharacterAndWait::LocationReached()
{
	IsLaunching = false;
	
	CMC->Velocity.Z = 0.0f;
	CMC->GravityScale = 0.0f;
	
	const FVector LaunchVelocity = FVector(0.0f, 0.0f, 750.0f - LaunchOffset);

	CMC->Velocity = FVector::ZeroVector; 
	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
	Character->LaunchCharacter(LaunchVelocity, true, true);

	GetWorld()->GetTimerManager().SetTimer(AerialStallTimerHandle, this, &UAT_LaunchCharacterAndWait::EndStall, StallDuration, false);
}

void UAT_LaunchCharacterAndWait::EndStall()
{
	CMC->GravityScale = 4.0f;

	OnLaunchComplete.Broadcast();
	EndTask();
}
