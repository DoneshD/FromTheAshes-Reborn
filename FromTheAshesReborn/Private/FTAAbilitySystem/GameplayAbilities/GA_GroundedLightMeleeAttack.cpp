#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedLightMeleeAttack.h"
#include "DataAsset/MeleeAttackDataAsset.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"


void UGA_GroundedLightMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	AttackMontageToPlay = LightAttacks[CurrentComboIndex]->MontageToPlay;

	PlayAttackMontage(AttackMontageToPlay);
}

void UGA_GroundedLightMeleeAttack::PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage)
{
		// AttackMontageToPlay = AttackMontage;
	 // Task = UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, AttackMontageToPlay, FGameplayTagContainer(),
	 // 1.0f, NAME_None, false, 1.0f);
	 // Task->OnBlendOut.AddDynamic(this, &UGA_GroundedLightMeleeAttack::OnCompleted);
	 // Task->OnCompleted.AddDynamic(this, &UGA_GroundedLightMeleeAttack::OnCompleted);
	 // Task->OnInterrupted.AddDynamic(this, &UGA_GroundedLightMeleeAttack::OnCancelled);
	 // Task->OnCancelled.AddDynamic(this, &UGA_GroundedLightMeleeAttack::OnCancelled);
	 // Task->EventReceived.AddDynamic(this, &UGA_GroundedLightMeleeAttack::EventReceived);
	 //
	//Task->ReadyForActivation();
	//Super::PlayAttackMontage(AttackMontageToPlay);
}

void UGA_GroundedLightMeleeAttack::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	//Debugging purposes, remove later
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

}

// void UGA_GroundedLightMeleeAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
// {
// 	Super::EventReceived(EventTag, EventData);
// 	UE_LOG(LogTemp, Warning, TEXT("UGA_GroundedLightMeleeAttack Event receieved"));
//
// }
