#include "FTAAbilitySystem/AbilityTasks/AT_SlamCharacterAndWait.h"

#include "CombatComponents/CentralStateComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAT_SlamCharacterAndWait* UAT_SlamCharacterAndWait::AT_SlamCharacterAndWait(UGameplayAbility* OwningAbility, FVector SlamLocation, float Speed, float Duration)
{
	UAT_SlamCharacterAndWait* Task = NewAbilityTask<UAT_SlamCharacterAndWait>(OwningAbility);
	Task->SlamLocation = SlamLocation;
	Task->SlamSpeed = Speed;
	Task->SlamDuration = Duration;
	return Task;

}

UAT_SlamCharacterAndWait::UAT_SlamCharacterAndWait(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

void UAT_SlamCharacterAndWait::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if(IsSlamming)
	{
		UpdateMovement(DeltaTime);
	}
}

void UAT_SlamCharacterAndWait::Activate()
{
	Super::Activate();

	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());

	if(!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAT_SlamCharacterAndWait::Activate - Character is Null"));
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

	SlamElapsedTime = 0.0f;
	SlamStartTime = GetWorld()->GetTimeSeconds();
	
	SlamStartLocation = GetAvatarActor()->GetActorLocation();
	SlamEndLocation = GetAvatarActor()->GetActorLocation() + FVector(0.0f, 0.0f, -SlamDownwardDistance);

	IsSlamming = true;
}

void UAT_SlamCharacterAndWait::ExternalCancel()
{
	Super::ExternalCancel();
}

FString UAT_SlamCharacterAndWait::GetDebugString() const
{
	return Super::GetDebugString();
}

void UAT_SlamCharacterAndWait::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);
}

void UAT_SlamCharacterAndWait::UpdateMovement(float DeltaTime)
{
	SlamElapsedTime += DeltaTime;

	const float Alpha = FMath::Clamp(SlamElapsedTime / SlamDuration, 0.0f, 1.0f);
	const FVector NewLocation = FMath::Lerp(SlamStartLocation, SlamLocation, Alpha);

	FHitResult Hit;
	GetAvatarActor()->SetActorLocation(NewLocation, true, &Hit);

	if (Hit.bBlockingHit)
	{
		LocationReached();
	}
}

void UAT_SlamCharacterAndWait::LocationReached()
{

	AFTACharacter* FTACharacter = Cast<AFTACharacter>(GetAvatarActor());
	if(FTACharacter)
	{
		FTACharacter->CentralStateComponent->SetCurrentOrientation(FTACharacter->CentralStateComponent->GroundedOrientationTag);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FTACharacter is NULL"));
	}

	
	IsSlamming = false;
	OnSlamComplete.Broadcast();
	EndTask();
	
}
