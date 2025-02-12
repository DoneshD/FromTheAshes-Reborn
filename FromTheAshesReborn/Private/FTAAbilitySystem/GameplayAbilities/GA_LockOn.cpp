#include "FTAAbilitySystem/GameplayAbilities/GA_LockOn.h"

#include "TargetSystemComponent.h"

UGA_LockOn::UGA_LockOn()
{
	
}

void UGA_LockOn::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UTargetSystemComponent* TargetSystemComponent = GetAvatarActorFromActorInfo()->FindComponentByClass<UTargetSystemComponent>();

	if(!TargetSystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetSystemComponent is NULL"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	
	if(IsTargeting)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	else
	{
		IsTargeting = TargetSystemComponent->TargetActor();
	}
}

void UGA_LockOn::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

}
