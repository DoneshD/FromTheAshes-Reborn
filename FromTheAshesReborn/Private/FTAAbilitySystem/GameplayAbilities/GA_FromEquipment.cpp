#include "FTAAbilitySystem/GameplayAbilities/GA_FromEquipment.h"
#include "Weapon/WeaponInstance.h"

UGA_FromEquipment::UGA_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

UWeaponInstance* UGA_FromEquipment::GetAssociatedWeaponInstance() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UWeaponInstance>(Spec->SourceObject.Get());
		
	}
	return nullptr;
}

