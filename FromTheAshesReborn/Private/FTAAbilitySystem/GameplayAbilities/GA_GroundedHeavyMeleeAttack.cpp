#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedHeavyMeleeAttack.h"

UGA_GroundedHeavyMeleeAttack::UGA_GroundedHeavyMeleeAttack()
{
}

void UGA_GroundedHeavyMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Grounded.Attacking.Melee.Light")));
	PerformGroundedMeleeAttack(HeavyAttackDataAssets);
}

void UGA_GroundedHeavyMeleeAttack::PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage)
{
	Super::PlayAttackMontage(AttackMontage);
}

void UGA_GroundedHeavyMeleeAttack::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}
