#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack_Aerial.h"

#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_MeleeWeaponAttack_Aerial::UGA_MeleeWeaponAttack_Aerial()
{
}

void UGA_MeleeWeaponAttack_Aerial::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
	
}

bool UGA_MeleeWeaponAttack_Aerial::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		return false;
	}

	return Character->GetCharacterMovement()->IsFalling() || Character->GetCharacterMovement()->IsFlying();
}

void UGA_MeleeWeaponAttack_Aerial::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	GetFTACharacterFromActorInfo()->GetCharacterMovement()->Velocity.Z = 0.0f;
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->GravityScale = 0.0f;

}

void UGA_MeleeWeaponAttack_Aerial::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_MeleeWeaponAttack_Aerial::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_MeleeWeaponAttack_Aerial::EndAirStall()
{
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->Velocity.Z = -100.0f;
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->GravityScale = 4.0f;
}

void UGA_MeleeWeaponAttack_Aerial::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_MeleeWeaponAttack_Aerial::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);
	EndAirStall();
}

void UGA_MeleeWeaponAttack_Aerial::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}
