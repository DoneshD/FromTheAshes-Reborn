#include "FTAAbilitySystem/GameplayAbilities/Dash/GA_Dash.h"

#include "ComboManagerComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_OnTick.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/InputReadingFunctionLibrary.h"
#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"

UGA_Dash::UGA_Dash()
{
	
}

void UGA_Dash::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);

	if(IsDashing)
	{
		UpdateDashMovement(DeltaTime);
	}
}

void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ELockOnInputOrientationDirection InputDirection = ELockOnInputOrientationDirection::Forward;

	AFTAPlayerState* PS = Cast<AFTAPlayerState>(ActorInfo->OwnerActor.Get());

	if(PS->HardLockedTargetActor)
	{
		float AngleOfInput = ULockOnFunctionLibrary::AngleFromInputVectorToLockedTarget(ActorInfo->AvatarActor.Get(), PS->HardLockedTargetActor);
		InputDirection = ULockOnFunctionLibrary::GetOrientationOfInput(AngleOfInput);
	}

	TObjectPtr<UDashAbilityDataAsset> MatchingDataAsset;
	bool DataAssetFound = FindMatchingDashAssetToInputDirection(DashAbilityAssets, MatchingDataAsset, InputDirection);

	if(!DataAssetFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Dash Asset not found"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	DashElapsedTime = 0.0f;
	DashStartTime = GetWorld()->GetTimeSeconds();

	FVector InputDir = UInputReadingFunctionLibrary::CheckInputVector(GetFTACharacterFromActorInfo()->GetCharacterMovement());

	DashStartLocation = ActorInfo->AvatarActor->GetActorLocation();
	DashEndLocation = ActorInfo->AvatarActor->GetActorLocation() + (InputDir * DashDistance);

	IsDashing = true;

	PlayAbilityAnimMontage(MatchingDataAsset->MontageToPlay);
	
}

bool UGA_Dash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Dash::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}

void UGA_Dash::UpdateDashMovement(float DeltaTime)
{
	DashElapsedTime += DeltaTime;
		
	float Alpha = FMath::Clamp(DashElapsedTime / DashDuration, 0.0f, 1.0f);
	FVector NewLocation = FMath::Lerp(DashStartLocation, DashEndLocation, Alpha);

	FHitResult Hit;
	CurrentActorInfo->AvatarActor->SetActorLocation(NewLocation, true, &Hit);

	if (Alpha >= 1.0f || Hit.bBlockingHit)
	{
		DashLocationReached();
	}
}

void UGA_Dash::DashLocationReached()
{
	IsDashing = false;
}

bool UGA_Dash::FindMatchingDashAssetToInputDirection(const TArray<UDashAbilityDataAsset*>& DashAbilityDataAssets, TObjectPtr<UDashAbilityDataAsset>& OutMatchingAbilityDataAsset, ELockOnInputOrientationDirection InputDirection)
{
	for (UDashAbilityDataAsset* CurrentAsset : DashAbilityDataAssets)
	{
		if (CurrentAsset)
		{
			if (CurrentAsset->Direction == InputDirection)
			{
				OutMatchingAbilityDataAsset = CurrentAsset;
				return true;
			}
		}
	}
	return false;
}

void UGA_Dash::PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage)
{
	Super::PlayAbilityAnimMontage(AnimMontage);
	
}

void UGA_Dash::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
	
}

void UGA_Dash::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);

	GetFTACharacterFromActorInfo()->ComboManagerComponent->GetCurrentComboContainer().Reset();
	GetFTACharacterFromActorInfo()->ComboManagerComponent->SetCurrentComboIndex(0);
	
}

void UGA_Dash::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
	
}
