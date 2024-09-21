#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedMeleeAttack.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"

UGA_GroundedMeleeAttack::UGA_GroundedMeleeAttack()
{

}

void UGA_GroundedMeleeAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_GroundedMeleeAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	//Use container instead?
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Event.Input.Saved.Light")))
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Light")));
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_GroundedMeleeAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{

}

void UGA_GroundedMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Event.Montage.Combo.Light.Open")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_GroundedMeleeAttack::ComboWindowTagChanged);
	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Event.Montage.Combo.Heavy.Open")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_GroundedMeleeAttack::ComboWindowTagChanged);
	
}

bool UGA_GroundedMeleeAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_GroundedMeleeAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_GroundedMeleeAttack::PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage)
{
	AttackMontageToPlay = AttackMontage;
	if(AttackMontageToPlay)
	{
		Task = UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, AttackMontageToPlay, FGameplayTagContainer(),
		1.0f, NAME_None, false, 1.0f);
		Task->OnBlendOut.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCompleted);
		Task->OnCompleted.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCompleted);
		Task->OnInterrupted.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCancelled);
		Task->OnCancelled.AddDynamic(this, &UGA_GroundedMeleeAttack::OnCancelled);
		Task->EventReceived.AddDynamic(this, &UGA_GroundedMeleeAttack::EventReceived);
		
		Task->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No attack montage"));
	}
}

void UGA_GroundedMeleeAttack::ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Montage.Combo.Light.Open"))))
	{
		GetWorld()->GetTimerManager().SetTimer(FComboWindowTimer, this, &UGA_GroundedMeleeAttack::CheckForLightInput, 0.1f, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(FComboWindowTimer);
	}
}

void UGA_GroundedMeleeAttack::CheckForLightInput()
{

	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Light"))))
	{
		UE_LOG(LogTemp, Warning, TEXT("SUCCESS!"));
		
	}
}


