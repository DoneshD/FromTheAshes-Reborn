#include "FTAAbilitySystem/GameplayAbilities/GA_WallRun.h"

#include "WallRunningComponent.h"
#include "FTACustomBase/FTACharacter.h"

UGA_WallRun::UGA_WallRun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_WallRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AFTACharacter* FTACharacter = GetFTACharacterFromActorInfo();
	
	if(!FTACharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_WallRun::ActivateAbility - Actor is NOT FTACharacter"))
		return;
	}

	FTACharacter->WallRunningComponent->WallRunningMovement();
}

bool UGA_WallRun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// // Check if the base ability can activate
	// if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	// {
	// 	return false;
	// }
	//
	// AFTACharacter* FTAChar = Cast<AFTACharacter>(ActorInfo->AvatarActor);
	//
	// if(!FTAChar)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("UGA_WallRun::CanActivateAbility - Actor is NOT FTACharacter"))
	// 	return false;
	// }
	return true;
}

void UGA_WallRun::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_WallRun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
