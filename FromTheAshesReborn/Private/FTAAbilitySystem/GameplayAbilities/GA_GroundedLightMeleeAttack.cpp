#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedLightMeleeAttack.h"

#include "AbilitySystemComponent.h"
#include "DataAsset/MeleeAttackDataAsset.h"


UGA_GroundedLightMeleeAttack::UGA_GroundedLightMeleeAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("Character.State.Grounded.Attacking.Melee.Light"));
}

void UGA_GroundedLightMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("Character.State.Grounded.Attacking.Melee.Heavy"));
	if (ComboTagContainer == LightAttackDataAssets[CurrentComboIndex]->RequiredTags)
	{
		if(CurrentComboIndex == LightAttackDataAssets[CurrentComboIndex]->RequiredIndex)
		{
			AttackMontageToPlay = LightAttackDataAssets[CurrentComboIndex]->MontageToPlay;
			FGameplayTag AttackIndentiferTag = LightAttackDataAssets[CurrentComboIndex]->AttackIndentiferTag;
			ComboTagContainer.AddTag(AttackIndentiferTag);
			UE_LOG(LogTemp, Warning, TEXT("LightAnim"));

			PlayAttackMontage(AttackMontageToPlay);
		}
	}
}

void UGA_GroundedLightMeleeAttack::PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage)
{
	Super::PlayAttackMontage(AttackMontage);
}

void UGA_GroundedLightMeleeAttack::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

}