#include "FTAAbilitySystem/GameplayAbilities/GA_Dash.h"

#include "AbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"
#include "FTACustomBase/FTACharacter.h"

UGA_Dash::UGA_Dash()
{
	AbilityInputID = EAbilityInputID::Dash;

}

void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData * TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	UAnimMontage* MontageToPlay = DashMontage;
	
	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dashing.Secondary"))))
	{
		MontageToPlay = BlinkMontage;
	}
	else
	{
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dashing.Initial")));
	}
	
	UFTAAT_PlayMontageAndWaitForEvent* Task = UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, FGameplayTagContainer(),
		1.0f, NAME_None, false, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &UGA_Dash::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UGA_Dash::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGA_Dash::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UGA_Dash::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UGA_Dash::EventReceived);
	
	Task->ReadyForActivation();
}

bool UGA_Dash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Dash::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		//CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UGA_Dash::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	//
	// GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dashing.Initial")));
	// GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dashing.Secondary")));
}

void UGA_Dash::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	
}

void UGA_Dash::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	
	
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dashing.Initial")));
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dashing.Secondary")));

	
}

void UGA_Dash::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	//EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}