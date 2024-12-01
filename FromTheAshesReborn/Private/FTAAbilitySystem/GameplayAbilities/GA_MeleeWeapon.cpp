#include "FTAAbilitySystem/GameplayAbilities/GA_MeleeWeapon.h"

#include "AbilitySystemComponent.h"
#include "Weapon/MeleeWeaponInstance.h"

UGA_MeleeWeapon::UGA_MeleeWeapon(const FObjectInitializer&)
{
	
}

UMeleeWeaponInstance* UGA_MeleeWeapon::GetMeleeWeaponInstance() const
{
	return Cast<UMeleeWeaponInstance>(GetAssociatedWeaponInstance());
}

bool UGA_MeleeWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	// if(bResult)
	// {
	// 	if(GetWeaponInstance() == nullptr)
	// 	{
	// 		if (GetWeaponInstance()== nullptr)
	// 		{
	// 			UE_LOG(LogTemp, Error, TEXT("Weapon ability %s cannot be activated because there is no associated ranged weapon (equipment instance=%s but needs to be derived from %s)"),
	// 				*GetPathName(),
	// 				*GetPathNameSafe(GetAssociatedEquipment()),
	// 				*UFTAEquipmentInstance::StaticClass()->GetName());
	// 			bResult = false;
	// 		}
	// 	}
	// }
	
	return bResult;
}

void UGA_MeleeWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* AbilitySystemComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(AbilitySystemComponent);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}


void UGA_MeleeWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


