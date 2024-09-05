#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedHeavyMeleeAttack.h"
#include "DataAsset/MeleeAttackDataAsset.h"


void UGA_GroundedHeavyMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (ComboTagContainer == HeavyAttacksDataAssets[CurrentComboIndex]->RequiredTags)
	{
		if(CurrentComboIndex == HeavyAttacksDataAssets[CurrentComboIndex]->RequiredIndex)
		{
			AttackMontageToPlay = HeavyAttacksDataAssets[CurrentComboIndex]->MontageToPlay;
			FGameplayTag AttackIndentiferTag = HeavyAttacksDataAssets[CurrentComboIndex]->AttackIndentiferTag;
			ComboTagContainer.AddTag(AttackIndentiferTag);
			UE_LOG(LogTemp, Warning, TEXT("HeavyAnim"));
			PlayAttackMontage(AttackMontageToPlay);

		}
	}

}

void UGA_GroundedHeavyMeleeAttack::PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage)
{
	AttackMontageToPlay = AttackMontage;
	Super::PlayAttackMontage(AttackMontageToPlay);
}

void UGA_GroundedHeavyMeleeAttack::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}
