#include "FTAAbilitySystem/GameplayAbilities/GA_LockOn.h"

#include "Player/FTAPlayerController.h"
#include "Player/FTAPlayerState.h"
#include "TargetingSystem/TargetingSystemComponent.h"

UGA_LockOn::UGA_LockOn()
{
	
}

bool UGA_LockOn::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	return bCanActivate;
}


void UGA_LockOn::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UTargetingSystemComponent* TargetingSystemComponent = GetAvatarActorFromActorInfo()->FindComponentByClass<UTargetingSystemComponent>();
	
	AFTAPlayerState* PS = GetFTAPlayerControllerFromActorInfo()->GetFTAPlayerState();

	if(!TargetingSystemComponent)
	{
		// UE_LOG(LogTemp, Error, TEXT("TargetSystemComponent is NULL"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	
	if(IsTargeting)
	{
		TargetingSystemComponent->TargetLockOff();
		IsTargeting = false;
		LockedOnTarget = nullptr;
		PS->HardLockedTargetActor = nullptr;
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	else
	{
		LockedOnTarget = TargetingSystemComponent->TargetActor(IsTargeting);
		if(LockedOnTarget)
		{
			PS->HardLockedTargetActor = LockedOnTarget;
		}
	}
}

void UGA_LockOn::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_LockOn::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	// if(this->IsActive())
	// {
	// 	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	// }
}

void UGA_LockOn::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	UTargetingSystemComponent* TargetingSystemComponent = GetAvatarActorFromActorInfo()->FindComponentByClass<UTargetingSystemComponent>();

	AFTAPlayerState* PS = GetFTAPlayerControllerFromActorInfo()->GetFTAPlayerState();

	if(!TargetingSystemComponent)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	
	TargetingSystemComponent->TargetLockOff();
	IsTargeting = false;
	LockedOnTarget = nullptr;
	PS->HardLockedTargetActor = nullptr;
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	

}
