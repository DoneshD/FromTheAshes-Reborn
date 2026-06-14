#include "FTAAbilitySystem/GameplayAbilities/Dash/GA_Dash.h"

#include "CombatComponents/ComboManagerComponent.h"
#include "DataAsset/MoveToLocationDataAsset.h"

#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_OnTick.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HelperFunctionLibraries/InputReadingFunctionLibrary.h"
#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UGA_Dash::UGA_Dash()
{
	
}

void UGA_Dash::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
	
}

bool UGA_Dash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		return false;
	}

	return !Character->GetCharacterMovement()->IsFalling() || !Character->GetCharacterMovement()->IsFlying();
	
}

void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AFTAPlayerState* PS = Cast<AFTAPlayerState>(ActorInfo->OwnerActor.Get());

	if(PS)
	{
		if(PS->HardLockedTargetActor)
		{
			FLockOnAngleResult AngleResult = ULockOnFunctionLibrary::AngleFromInputVectorToLockedTarget(ActorInfo->AvatarActor.Get(), PS->HardLockedTargetActor);
			InputDirection = ULockOnFunctionLibrary::GetOrientationOfInput(AngleResult);
		}	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_Dash::ActivateAbility - Player state null"))
	}

	CurrentDashData = DuplicateObject<UDashAbilityDataAsset>(DefaultDashData,this);
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}


void UGA_Dash::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	
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

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

	
}

void UGA_Dash::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
	
}

UFTAAbilityDataAsset* UGA_Dash::SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets)
{
	Super::SelectAbilityAsset(InAbilityAssets);

	TArray<UDashAbilityDataAsset*> DashDataAssets;

	for (UFTAAbilityDataAsset* Asset : InAbilityAssets)
	{
		if (UDashAbilityDataAsset* DashAsset = Cast<UDashAbilityDataAsset>(Asset))
		{
			DashDataAssets.Add(DashAsset);
		}
	}

	if(TObjectPtr<UDashAbilityDataAsset> AbilityDataAsset = ComboManagerComponent->GetDashAssetByRequirements(DashDataAssets, InputDirection))
	{
		return AbilityDataAsset;
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
	return nullptr;

}

void UGA_Dash::ExtractAssetProperties(UFTAAbilityDataAsset* InAbilityAsset)
{
	Super::ExtractAssetProperties(InAbilityAsset);

	UDashAbilityDataAsset* DashAsset = Cast<UDashAbilityDataAsset>(InAbilityAsset);

	if(DashAsset)
	{
		if(DashAsset->DashMoveToLocationDataAsset)
		{
			CurrentMoveToLocationAsset = DashAsset->DashMoveToLocationDataAsset;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Here 2"))
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Here 1"))
	}
}

void UGA_Dash::SetRuntimeAbilityData(UFTAAbilityDataAsset* InAbilityRuntimeData)
{
	Super::SetRuntimeAbilityData(InAbilityRuntimeData);
}

void UGA_Dash::PerformAbility(UFTAAbilityDataAsset* InAbilityAsset)
{
	Super::PerformAbility(InAbilityAsset);
}
