#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedHeavyMeleeAttack.h"
#include "DataAsset/MeleeAttackDataAsset.h"


UGA_GroundedHeavyMeleeAttack::UGA_GroundedHeavyMeleeAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("Character.State.Grounded.Attacking.Melee.Heavy"));
}

void UGA_GroundedHeavyMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("Character.State.Grounded.Attacking.Melee.Light"));

	for (const FGameplayTag& Tag : ComboTagContainer)
	{
		UE_LOG(LogTemp, Warning, TEXT(" - %s"), *Tag.ToString());
	}
	UE_LOG(LogTemp, Warning, TEXT("=============================================="));
	for (const FGameplayTag& NewTag : HeavyAttackDataAssets[CurrentComboIndex]->RequiredTags)
	{
		UE_LOG(LogTemp, Warning, TEXT(" - %s"), *NewTag.ToString());
	}
	
	if (ComboTagContainer == HeavyAttackDataAssets[CurrentComboIndex]->RequiredTags)
	{
		if(CurrentComboIndex == HeavyAttackDataAssets[CurrentComboIndex]->RequiredIndex)
		{
			AttackMontageToPlay = HeavyAttackDataAssets[CurrentComboIndex]->MontageToPlay;
			FGameplayTag AttackIndentiferTag = HeavyAttackDataAssets[CurrentComboIndex]->AttackIndentiferTag;
			ComboTagContainer.AddTag(AttackIndentiferTag);
			UE_LOG(LogTemp, Warning, TEXT("HeavyAnim"));

			PlayAttackMontage(AttackMontageToPlay);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Why"));			
	}
}

void UGA_GroundedHeavyMeleeAttack::PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage)
{
	Super::PlayAttackMontage(AttackMontage);
}

void UGA_GroundedHeavyMeleeAttack::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}
