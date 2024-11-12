#include "FTAAbilitySystem/GameplayAbilities/GA_FromEquipment.h"
#include "Weapons/FTAEquipmentInstance.h"

UGA_FromEquipment::UGA_FromEquipment(const FObjectInitializer& ObjectInitializer)
{
	
}

UFTAEquipmentInstance* UGA_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UFTAEquipmentInstance>(Spec->SourceObject.Get());
		
	}

	return nullptr;
}