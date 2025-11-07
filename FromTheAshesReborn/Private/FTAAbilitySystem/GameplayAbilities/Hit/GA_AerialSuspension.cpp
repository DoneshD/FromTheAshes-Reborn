#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_AerialSuspension.h"

#include "MaterialHLSLTree.h"
#include "CombatComponents/AerialCombatComponent.h"
#include "CombatComponents/CentralStateComponent.h"
#include "EventObjects/SuspendEventObject.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/AT_SuspendInAirAndWait.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_AerialSuspension::UGA_AerialSuspension()
{
	ReceiveHitTag = FGameplayTag::RequestGameplayTag("HitTag.Effect.GrantAbility.Suspend");
	
}

void UGA_AerialSuspension::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
}

bool UGA_AerialSuspension::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
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
	

	if(Character->GetCharacterMovement()->IsWalking())
	{
		return false;
	}

	return true;
}

void UGA_AerialSuspension::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const USuspendEventObject* SuspendEventObj = Cast<USuspendEventObject>(CurrentEventData.OptionalObject);
	
	/*if(!SuspendEventObj)
	{
		UE_LOG(LogTemp, Error, TEXT("SuspendEventObj is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
		
	}
	
	SuspendTask = UAT_SuspendInAirAndWait::AT_SuspendInAirAndWait(this,
	SuspendEventObj->SuspendData.DescentSpeed,
	5.0f);;
	
	if (SuspendTask)
	{
		SuspendTask->OnSuspendComplete.AddDynamic(this, &UGA_AerialSuspension::OnSuspendComplete);
		SuspendTask->ReadyForActivation();
	}

	*/
	

	//-----------


	if(!GetFTAAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Flail")))
	{
		GetFTAAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Flail"));
	}

	UAerialCombatComponent* ACC = GetFTACharacterFromActorInfo()->FindComponentByClass<UAerialCombatComponent>();
	
	if(ACC->EnableAerialCombatEffect)
	{
		FGameplayEffectSpecHandle GEHandle = MakeOutgoingGameplayEffectSpec(ACC->EnableAerialCombatEffect, 1.0f);
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
	}
	
	if(ACC && ACC->IsValidLowLevel())
	{
		if(ACC->AddAerialCombatGravity)
		{
			UCentralStateComponent* CSC = GetFTACharacterFromActorInfo()->FindComponentByClass<UCentralStateComponent>();
			if(CSC)
			{
				CSC->SetCurrentOrientation(CSC->AirborneOrientationTag, MOVE_Falling);
			}
			if(ACC->AddAerialCombatGravity)
			{
				FGameplayEffectSpecHandle GEHandle = MakeOutgoingGameplayEffectSpec(ACC->AddAerialCombatGravity, 1.0f);
				GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
			}
			
		}
	}
}

void UGA_AerialSuspension::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_AerialSuspension::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_AerialSuspension::OnSuspendComplete()
{
	// EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_AerialSuspension::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_AerialSuspension::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);

}

void UGA_AerialSuspension::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}
