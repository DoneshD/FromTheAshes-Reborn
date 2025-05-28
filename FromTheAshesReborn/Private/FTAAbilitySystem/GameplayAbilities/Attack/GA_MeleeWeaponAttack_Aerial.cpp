#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack_Aerial.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ComboManagerComponent.h"
#include "EventObjects/SuspendEventObject.h"
#include "FTAAbilitySystem/AbilityTasks/AT_SuspendInAirAndWait.h"
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

	ComboManagerComponent->AerialAttacksCounter += 1;

	SuspendTask = UAT_SuspendInAirAndWait::AT_SuspendInAirAndWait(this,
		DescentSpeed,
		5.0f);

	SuspendTask->ReadyForActivation();
	
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


void UGA_MeleeWeaponAttack_Aerial::OnHitAdded(FHitResult LastItem)
{
	Super::OnHitAdded(LastItem);

	AActor* TargetActor = LastItem.GetActor();

	if (TargetActor && TargetActor->Implements<UAbilitySystemInterface>())
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(TargetActor);
		UAbilitySystemComponent* TargetASC = AbilitySystemInterface->GetAbilitySystemComponent();

		if (TargetASC)
		{
			FGameplayEventData EventData;
			
			EventData.Instigator = GetAvatarActorFromActorInfo();
			EventData.Target = TargetActor;
			EventData.ContextHandle.AddHitResult(LastItem);
			EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("AbilityTag.Suspend"));

			//------------------------------------------------------------------------------------------------

			USuspendEventObject* SuspendEventObj = NewObject<USuspendEventObject>(this);
			SuspendEventObj->SuspendData.DescentSpeed = DescentSpeed;

			EventData.OptionalObject = SuspendEventObj;
			
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, EventData.EventTag, EventData);

		}
	}
}

void UGA_MeleeWeaponAttack_Aerial::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_MeleeWeaponAttack_Aerial::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);

	if(SuspendTask)
	{
		SuspendTask->EndTask();
	}
}

void UGA_MeleeWeaponAttack_Aerial::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}
