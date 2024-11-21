#include "Weapons/FTAWeaponInstance.h"

UFTAWeaponInstance::UFTAWeaponInstance(const FObjectInitializer& ObjectInitializer)
{
	if (APawn* Pawn = GetPawn())
	{
		if (Pawn->IsPlayerControlled())
		{
			
		}
	}
}

void UFTAWeaponInstance::OnEquipped()
{
	Super::OnEquipped();
}

void UFTAWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}

TSubclassOf<UAnimInstance> UFTAWeaponInstance::PickBestAnimLayer(bool bEquipped,
	const FGameplayTagContainer& CosmeticTags) const
{
	const FFTAAnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UnEquippedAnimSet);
	return SetToQuery.SelectBestLayer(CosmeticTags);
}



