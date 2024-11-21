#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FTAEquipmentInstance.generated.h"

class AActor;
class APawn;
struct FFTAEquipmentActorToSpawn;

UCLASS(BlueprintType, Blueprintable)

class FROMTHEASHESREBORN_API UFTAEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	UFTAEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category=Equipment)
	UObject* GetInstigator() const { return Instigator; }

	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }

	UFUNCTION(BlueprintPure, Category=Equipment)
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	UFUNCTION(BlueprintPure, Category=Equipment)
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	virtual void SpawnEquipmentActors(const TArray<FFTAEquipmentActorToSpawn>& ActorsToSpawn);
	virtual void DestroyEquipmentActors();

	virtual void OnEquipped();
	virtual void OnUnequipped();

private:

	UPROPERTY()
	TObjectPtr<UObject> Instigator;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedActors;
	
};

