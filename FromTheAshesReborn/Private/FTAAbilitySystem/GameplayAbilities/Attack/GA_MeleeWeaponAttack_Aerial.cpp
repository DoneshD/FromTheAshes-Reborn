#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack_Aerial.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CombatComponents/ComboManagerComponent.h"
#include "CombatComponents/AerialCombatComponent.h"
#include "EventObjects/SuspendEventObject.h"
#include "FTAAbilitySystem/AbilityTasks/AT_SuspendInAirAndWait.h"
#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_ReceiveHit.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
#include "TargetingSystem/TargetingSystemComponent.h"

UGA_MeleeWeaponAttack_Aerial::UGA_MeleeWeaponAttack_Aerial()
{
}

void UGA_MeleeWeaponAttack_Aerial::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
}

bool UGA_MeleeWeaponAttack_Aerial::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!UFTAGameplayAbility::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
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
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);\

	GetFTACharacterFromActorInfo()->GetCharacterMovement()->Velocity.Z = 0;
	
	// SuspendTask = UAT_SuspendInAirAndWait::AT_SuspendInAirAndWait(this,
	// 	DescentSpeed,
	// 	5.0f);

	// SuspendTask->ReadyForActivation();

	AerialCombatComponent = FTAChar->FindComponentByClass<UAerialCombatComponent>();

	if(!AerialCombatComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack_Aerial::ActivateAbility - AerialCombatComponent"));
		return;
	}
	
	if(EnableAerialCombatEffect)
	{
		FGameplayEffectSpecHandle GEHandle = MakeOutgoingGameplayEffectSpec(EnableAerialCombatEffect, 1.0f);
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
	}

	if(AddAerialCombatGravity)
	{
		FGameplayEffectSpecHandle GEHandle = MakeOutgoingGameplayEffectSpec(AddAerialCombatGravity, 1.0f);
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
	}
	
}

void UGA_MeleeWeaponAttack_Aerial::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_MeleeWeaponAttack_Aerial::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	//TODO: This is being called from a grounded attack, check later 
}

void UGA_MeleeWeaponAttack_Aerial::SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass)
{
	USuspendEventObject* SuspendInfoObj = NewObject<USuspendEventObject>(this);
	SuspendInfoObj->SuspendData.DescentSpeed = DescentSpeed;
	SuspendInfoObj->HitData.Instigator = GetFTACharacterFromActorInfo();
	SuspendInfoObj->HitData.HitDirection = FinalAttackData.AttackDirection;

	OnHitEventData.OptionalObject = SuspendInfoObj;

	Super::SendMeleeHitGameplayEvents(TargetDataHandle, InHitAbilityClass);
}


void UGA_MeleeWeaponAttack_Aerial::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_MeleeWeaponAttack_Aerial::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);
	
}

void UGA_MeleeWeaponAttack_Aerial::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}

void UGA_MeleeWeaponAttack_Aerial::OnMontageBlendingOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageBlendingOut(EventTag, EventData);

	if(SuspendTask)
	{
		SuspendTask->EndTask();
	}
}
