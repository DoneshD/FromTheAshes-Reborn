#include "UI/UW_EquipmentWheel.h"

#include "UI/UW_EquipmentItemSlot.h"

UUW_EquipmentItemSlot* UUW_EquipmentWheel::GetEquippedItemSlot(TArray<UUW_EquipmentItemSlot*> SlotArray) const
{
	for (int i = 0; i < SlotArray.Num(); i++)
	{
		UUW_EquipmentItemSlot* Item = SlotArray[i];
		if(Item)
		{
			if(Item->IsEquipped)
			{
				return Item;
			}
			
		}
	}
	return nullptr;
}

void UUW_EquipmentWheel::SetEquippedItemSlot(UUW_EquipmentItemSlot* ItemSlot) const
{
	ItemSlot->IsEquipped = true;
}
