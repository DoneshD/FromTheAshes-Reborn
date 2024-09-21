#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedLightMeleeAttack.h"

#include "AbilitySystemComponent.h"
#include "DataAsset/MeleeAttackDataAsset.h"


UGA_GroundedLightMeleeAttack::UGA_GroundedLightMeleeAttack()
{
}

void UGA_GroundedLightMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	AttackMontageToPlay = LightAttackDataAssets[0]->MontageToPlay;
	PlayAttackMontage(AttackMontageToPlay);

}

void UGA_GroundedLightMeleeAttack::PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage)
{
	Super::PlayAttackMontage(AttackMontage);
	
}

void UGA_GroundedLightMeleeAttack::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

}