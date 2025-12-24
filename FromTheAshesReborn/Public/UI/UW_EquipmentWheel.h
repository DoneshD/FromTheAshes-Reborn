#pragma once

#include "CoreMinimal.h"
#include "FTAUserWidget.h"
#include "UW_EquipmentWheel.generated.h"

class UUW_EquipmentItemSlot;

UCLASS()
class FROMTHEASHESREBORN_API UUW_EquipmentWheel : public UFTAUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UUW_EquipmentItemSlot>> AscendantSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UUW_EquipmentItemSlot>> AbyssalSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UUW_EquipmentItemSlot>> ZenithSlots;
public:
	

	UFUNCTION(BlueprintCallable)
	UUW_EquipmentItemSlot* GetEquippedItemSlot(TArray<UUW_EquipmentItemSlot*> SlotArray) const;

	UFUNCTION(BlueprintCallable)
	void SetEquippedItemSlot(UUW_EquipmentItemSlot* ItemSlot) const;
	
};
