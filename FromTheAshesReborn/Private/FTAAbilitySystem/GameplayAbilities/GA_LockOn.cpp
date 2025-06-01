#include "FTAAbilitySystem/GameplayAbilities/GA_LockOn.h"

#include "Player/FTAPlayerController.h"
#include "Player/FTAPlayerState.h"
#include "TargetingSystem/TargetingSystemComponent.h"

UGA_LockOn::UGA_LockOn()
{
	
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
			UE_LOG(LogTemp, Warning, TEXT("GALOCKON:: LockedOnTarget success"))

		}
	}
}

void UGA_LockOn::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	if(this->IsActive())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_LockOn::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

}
