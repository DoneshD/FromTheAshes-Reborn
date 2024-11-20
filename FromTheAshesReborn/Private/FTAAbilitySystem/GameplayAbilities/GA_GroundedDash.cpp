#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedDash.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"
#include "DataAsset/DashDataAsset.h"
#include "Interfaces/PlayerComboManagerInterface.h"
#include "Player/FTAPlayerController.h"

UGA_GroundedDash::UGA_GroundedDash()
{
	
}

void UGA_GroundedDash::ResetDash()
{
	IPlayerComboManagerInterface* PlayerComboManagerInterface = Cast<IPlayerComboManagerInterface>(GetAvatarActorFromActorInfo());
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}


//TODO: FIX later
void UGA_GroundedDash::PerformDash()
{
	IPlayerComboManagerInterface* PlayerComboManagerInterface = Cast<IPlayerComboManagerInterface>(GetAvatarActorFromActorInfo());

	if(DashDataAssets.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("DashDataAssets is empty"));
		return;
	}
	FGameplayTag DashIndentiferTag;
	DashIndentiferTag = DashDataAssets[0]->DashIndentiferTag;
	DashMontageToPlay = DashDataAssets[0]->MontageToPlay;

	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Dash"))))
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Dash")));
		UE_LOG(LogTemp, Warning, TEXT("PerformDash Inside"));
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
	if(GetAvatarActorFromActorInfo()->GetInstigatorController())
	{
		PC = Cast<AFTAPlayerController>(GetAvatarActorFromActorInfo()->GetInstigatorController());
		if(!PC)
		{
			UE_LOG(LogTemp, Warning, TEXT("!PC"));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("!GetAvatarActorFromActorInfo()->GetInstigatorController()"));
		return;
	}
	
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
	ResetDash();
}

void UGA_GroundedDash::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}
