#pragma once

#include "CoreMinimal.h"
#include "FTAUserWidget.h"
#include "UW_EquipmentItemSlot.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UUW_EquipmentItemSlot : public UFTAUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	int32 SlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	bool IsEquipped = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	bool IsSelected  = false;

	

	
};
