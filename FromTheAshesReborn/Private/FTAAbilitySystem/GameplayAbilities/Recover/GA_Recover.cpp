#include "FTAAbilitySystem/GameplayAbilities/Recover/GA_Recover.h"

#include "CombatComponents/CentralStateComponent.h"
#include "DataAsset/RecoverAbilityDataAsset.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Recover::UGA_Recover(const FObjectInitializer&)
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag("EffectTag.GrantRecovery.GetUp");
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::OwnedTagAdded;
	AbilityTriggers.Add(TriggerData);
}

void UGA_Recover::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
}

bool UGA_Recover::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Recover::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// GetFTACharacterFromActorInfo()->CentralStateComponent->SetCurrentState(FGameplayTag::RequestGameplayTag("Character.State.Neutral"));
	GetFTACharacterFromActorInfo()->CentralStateComponent->SetCurrentOrientation(GetFTACharacterFromActorInfo()->CentralStateComponent->GroundedOrientationTag, MOVE_Walking);
	GetFTAAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("DownedCombatTag.EnableComponent")));
	GetFTAAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("AerialCombatTag.EnableComponent")));
	
	if(RecoverAbilityAsset)
	{
		if(RecoverAbilityAsset->MontageToPlay)
		{
			PlayAbilityAnimMontage(RecoverAbilityAsset->MontageToPlay);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MontageToPlay null"))
	
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Recover null"))
	}

	GetFTACharacterFromActorInfo()->GetCharacterMovement()->DisableMovement();
}

void UGA_Recover::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Recover::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	
}

void UGA_Recover::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_Recover::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
	
}

void UGA_Recover::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}
