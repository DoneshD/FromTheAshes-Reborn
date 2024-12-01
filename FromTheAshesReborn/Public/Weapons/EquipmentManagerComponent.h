
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FTAAbilitySystem/AbilitySets/FTAAbilitySet.h"
#include "EquipmentManagerComponent.generated.h"

class UWeaponInstance;
class UWeaponDefinition;
class UFTAEquipmentInstance;
class UFTAEquipmentDefinition;

USTRUCT(BlueprintType)
struct FFTAAppliedEquipmentEntry
{
	GENERATED_BODY()

	FFTAAppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

private:
	friend UEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UWeaponDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UWeaponInstance> Instance = nullptr;

	// Authority-only list of granted handles
	FFTAAbilitySet_GrantedHandles GrantedHandles;
};

UCLASS(BlueprintType, Const)
class FROMTHEASHESREBORN_API UEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FFTAAppliedEquipmentEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;

public:
	UEquipmentManagerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	UWeaponInstance* AddEntry(TSubclassOf<UWeaponDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable)
	void RemoveEntry(UWeaponInstance* Instance);

	UFTAAbilitySystemComponent* GetAbilitySystemComponent() const;
};
