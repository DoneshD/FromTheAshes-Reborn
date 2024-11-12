#pragma once

#include "CoreMinimal.h"
#include "FTAEquipmentInstance.h"
#include "Weapons/FTACosmeticAnimationTypes.h"
#include "FTAWeaponInstance.generated.h"

class UAnimInstance;
class UObject;
struct FFrame;
struct FGameplayTagContainer;

UCLASS()
class FROMTHEASHESREBORN_API UFTAWeaponInstance : public UFTAEquipmentInstance
{
	GENERATED_BODY()

public:
	
	UFTAWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	FFTAAnimLayerSelectionSet EquippedAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	FFTAAnimLayerSelectionSet UnEquippedAnimSet;

	// UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Animation)
	// TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;

	/** Callback for when the owning pawn of this weapon dies. Removes all spawned device properties. */
	UFUNCTION()
	void OnDeathStarted(AActor* OwningActor);
	
};
