#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_Launched.h"

#include "CombatComponents/AerialCombatComponent.h"
#include "CombatComponents/CentralStateComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "Kismet/GameplayStatics.h"

UGA_Launched::UGA_Launched()
{
	ReceiveHitTag = FGameplayTag::RequestGameplayTag("HitTag.Effect.GrantAbility.Launch");
}

void UGA_Launched::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
}

bool UGA_Launched::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Launched::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	GetFTAAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("DownedCombatTag.EnableComponent")));
	GetFTAAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("Character.State.Downed"));

	UCentralStateComponent* CSC = GetFTACharacterFromActorInfo()->FindComponentByClass<UCentralStateComponent>();
	if(CSC)
	{
		CSC->SetCurrentOrientation(CSC->AirborneOrientationTag, MOVE_Falling);
	}
	
}

void UGA_Launched::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Launched::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	GetFTAAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Flail"));
	

	UAerialCombatComponent* AerialCombatComponent = GetFTACharacterFromActorInfo()->FindComponentByClass<UAerialCombatComponent>();

	if(AerialCombatComponent && AerialCombatComponent->IsValidLowLevel())
	{
		// if(AerialCombatComponent->EnableAerialCombatEffect)
		// {
		// 	FGameplayEffectSpecHandle GEHandle = MakeOutgoingGameplayEffectSpec(AerialCombatComponent->EnableAerialCombatEffect, 1.0f);
		// 	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
		// }
		if(!GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("AerialCombatTag.EnableComponent")))
		{
			GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("AerialCombatTag.EnableComponent"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid pointer or null"))
	}
	
	if (GetFTAAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(ReceiveHitTag))
	{
		GetFTAAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(ReceiveHitTag));
	}
	
}