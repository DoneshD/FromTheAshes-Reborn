#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedDash.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"
#include "DataAsset/DashDataAsset.h"

UGA_GroundedDash::UGA_GroundedDash()
{
	
}

void UGA_GroundedDash::ResetDash()
{
	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(DashWindowTag,
			EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(DashInputTag);

	GetWorld()->GetTimerManager().ClearTimer(FDashComboWindowTimer);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_GroundedDash::DashWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(DashWindowTag))
	{
		GetWorld()->GetTimerManager().SetTimer(FDashComboWindowTimer, this, &UGA_GroundedDash::DashWindowTagOpen, 0.01f, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(FDashComboWindowTimer);
	}
}

void UGA_GroundedDash::DashWindowTagOpen()
{
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(DashInputTag))
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(DashInputTag);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponentFromActorInfo()->GetActivatableAbilities())
		{
			if (Spec.Ability)
			{
				if(Spec.InputID == 6)
				{
					GetAbilitySystemComponentFromActorInfo()->TryActivateAbility(Spec.Handle);
				}
			}
		}
	}
}

void UGA_GroundedDash::PerformDash()
{
	if(DashDataAssets.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("DashDataAssets is empty"));
		return;
	}
	FGameplayTag DashIndentiferTag;
	DashMontageToPlay = DashDataAssets[0]->MontageToPlay;
	DashIndentiferTag = DashDataAssets[0]->DashIndentiferTag;
	
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(DashInputTag))
	{
		DashMontageToPlay = DashDataAssets[1]->MontageToPlay;
		DashIndentiferTag = DashDataAssets[1]->DashIndentiferTag;
	}
	
	if(DashMontageToPlay)
	{
		// MeleeCombatantInterface->GetCurrentComboContainer().AddTag(DashIndentiferTag);
		// MeleeCombatantInterface->SetCurrentComboIndex(MeleeCombatantInterface->GetCurrentComboIndex() + 1);
		PlayDashMontage();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DashMontageToPlay is NULL"));
		
	}
}

void UGA_GroundedDash::PlayDashMontage()
{
	if(DashMontageToPlay)
	{
		Task = UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, DashMontageToPlay, FGameplayTagContainer(),
		1.0f, NAME_None, false, 1.0f);
		Task->OnBlendOut.AddDynamic(this, &UGA_GroundedDash::OnCompleted);
		Task->OnCompleted.AddDynamic(this, &UGA_GroundedDash::OnCompleted);
		Task->OnInterrupted.AddDynamic(this, &UGA_GroundedDash::OnCancelled);
		Task->OnCancelled.AddDynamic(this, &UGA_GroundedDash::OnCancelled);
		Task->EventReceived.AddDynamic(this, &UGA_GroundedDash::EventReceived);
		
		Task->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No attack montage"));
	}
}

void UGA_GroundedDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(DashWindowTag,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_GroundedDash::DashWindowTagChanged);

	PerformDash();
}

bool UGA_GroundedDash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_GroundedDash::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_GroundedDash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_GroundedDash::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_GroundedDash::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_GroundedDash::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}
