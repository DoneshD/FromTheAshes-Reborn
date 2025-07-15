#include "FTAAbilitySystem/GameplayAbilities/GA_LockOn.h"

#include "FTACustomBase/FTACharacter.h"
#include "Player/FTAPlayerController.h"
#include "Player/FTAPlayerState.h"
#include "TargetingSystem/TargetingSystemComponent.h"

UGA_LockOn::UGA_LockOn()
{
	
}

void UGA_LockOn::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);

	if(TargetCharacter)
	{
		if(TargetCharacter->IsDead || TargetCharacter->IsAlreadyDead)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
			return;
		}
	}
	
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
	
	LockedOnTarget = TargetingSystemComponent->TargetActor(IsTargeting);
	if(LockedOnTarget)
	{
		PS->HardLockedTargetActor = LockedOnTarget;
		TargetCharacter = Cast<AFTACharacter>(LockedOnTarget);
	}

	if(TargetCharacter)
	{
		if(TargetCharacter->IsDead || TargetCharacter->IsAlreadyDead)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
			return;
		}
	}
}

void UGA_LockOn::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UTargetingSystemComponent* TargetingSystemComponent = GetAvatarActorFromActorInfo()->FindComponentByClass<UTargetingSystemComponent>();
	AFTAPlayerState* PS = GetFTAPlayerControllerFromActorInfo()->GetFTAPlayerState();

	TargetingSystemComponent->TargetLockOff();
	IsTargeting = false;
	LockedOnTarget = nullptr;
	PS->HardLockedTargetActor = nullptr;
}

void UGA_LockOn::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
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
