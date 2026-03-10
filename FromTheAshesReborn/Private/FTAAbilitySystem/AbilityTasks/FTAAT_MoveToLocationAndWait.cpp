#include "FTAAbilitySystem/AbilityTasks/FTAAT_MoveToLocationAndWait.h"

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
		UpdateMovement(DeltaTime);
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

void UFTAAT_MoveToLocationAndWait::UpdateMovement(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Moving"));
}

void UFTAAT_MoveToLocationAndWait::LocationReached()
{
}
