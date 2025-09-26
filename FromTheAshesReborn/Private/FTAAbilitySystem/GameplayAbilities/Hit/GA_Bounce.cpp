
#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_Bounce.h"

#include "AbilitySystemComponent.h"
#include "CombatComponents/AerialCombatComponent.h"
#include "FTACustomBase/FTACharacter.h"

UGA_Bounce::UGA_Bounce()
{
}

void UGA_Bounce::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
}

bool UGA_Bounce::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Bounce::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAerialCombatComponent* ACC = GetFTACharacterFromActorInfo()->FindComponentByClass<UAerialCombatComponent>();

	ACC->ChangeMovementMode(MOVE_Flying);

	if(EnableAerialCombatEffect)
	{
		FGameplayEffectSpecHandle GEHandle = MakeOutgoingGameplayEffectSpec(EnableAerialCombatEffect, 1.0f);
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
	}
}

void UGA_Bounce::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Bounce::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}

void UGA_Bounce::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_Bounce::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);
}

void UGA_Bounce::OnMontageBlendingOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageBlendingOut(EventTag, EventData);

	if(EnableDownedCombatEffect)
	{
		FGameplayEffectSpecHandle GEHandle = MakeOutgoingGameplayEffectSpec(EnableDownedCombatEffect, 1.0f);
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
	}
}

void UGA_Bounce::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}
