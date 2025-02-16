#include "FTAAbilitySystem/GameplayAbilities/GA_MeleeWeaponAttack_Launcher.h"
#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"
#include "Player/FTAPlayerController.h"
#include "Player/FTAPlayerState.h"

bool UGA_MeleeWeaponAttack_Launcher::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// UE_LOG(LogTemp, Warning, TEXT("UGA_MeleeWeaponAttack_Launcher"));

	if(!ActorInfo->OwnerActor.Get())
	{
		UE_LOG(LogTemp, Error, TEXT("ActorInfo->OwnerActor.Get( NULL"));
		return false;
	}

	if(!ActorInfo->AvatarActor.Get())
	{
		UE_LOG(LogTemp, Error, TEXT("ActorInfo->AvatarActor.Get( NULL"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("OwnerActor Name: %s"), *ActorInfo->OwnerActor.Get()->GetName());
	UE_LOG(LogTemp, Warning, TEXT("AvatarActor Name: %s"), *ActorInfo->AvatarActor.Get()->GetName());


	// AFTAPlayerState* PS = Cast<AFTAPlayerState>(ActorInfo->OwnerActor);
	// if(!PS)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("PS NULL"));
	// 	return false;
	// }
	// if(!ActorInfo->AvatarActor.Get())
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("ActorInfo->AvatarActor.Get() NULL"));
	// 	return false;
	// }
	//
	// if(!PS->HardLockedTargetActor.Get())
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("PS->HardLockedTargetActor.Get() NULL"));
	// 	return false;
	// }

	// float AngleOfInput = ULockOnFunctionLibrary::AngleFromLockedTarget(ActorInfo->AvatarActor.Get(), GetFTAPlayerControllerFromActorInfo()->GetFTAPlayerState()->HardLockedTargetActor);

	// UE_LOG(LogTemp, Warning, TEXT("UGA_MeleeWeaponAttack_Launcher value: %f"), AngleOfInput);
	
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	
}

void UGA_MeleeWeaponAttack_Launcher::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

}
