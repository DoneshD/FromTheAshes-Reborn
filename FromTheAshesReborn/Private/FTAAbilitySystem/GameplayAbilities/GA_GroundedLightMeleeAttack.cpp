#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedLightMeleeAttack.h"
#include "DataAsset/MeleeAttackDataAsset.h"


void UGA_GroundedLightMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ComboTagContainer == LightAttackDataAssets[CurrentComboIndex]->RequiredTags)
	{
		if(CurrentComboIndex == LightAttackDataAssets[CurrentComboIndex]->RequiredIndex)
		{
			
			AttackMontageToPlay = LightAttackDataAssets[CurrentComboIndex]->MontageToPlay;
			FGameplayTag AttackIndentiferTag = LightAttackDataAssets[CurrentComboIndex]->AttackIndentiferTag;
			ComboTagContainer.AddTag(AttackIndentiferTag);
			UE_LOG(LogTemp, Warning, TEXT("LightAnim"));
			if(LightAttackDataAssets[CurrentComboIndex]->IsComboFinisher)
			{
				CurrentComboIndex = 0;
			}
			UE_LOG(LogTemp, Warning, TEXT("Index: %d"), CurrentComboIndex);

			PlayAttackMontage(AttackMontageToPlay);

		}
	}

}

void UGA_GroundedLightMeleeAttack::PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage)
{
	AttackMontageToPlay = AttackMontage;
	Super::PlayAttackMontage(AttackMontageToPlay);
}

void UGA_GroundedLightMeleeAttack::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	//Debugging purposes, remove later

}