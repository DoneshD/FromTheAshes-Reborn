﻿
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
struct FFTAAppliedEquipmentItem
{
	GENERATED_BODY()

	FFTAAppliedEquipmentItem()
	{}

	FString GetDebugString() const;

public:
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
	TArray<FFTAAppliedEquipmentItem> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;

	UWeaponInstance* CurrentEquippedWeaponInstance;

public:
	UEquipmentManagerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	void RemoveEquipmentItem(UWeaponInstance* Instance);
	
	UFUNCTION(BlueprintCallable)
	UWeaponInstance* SetEquippedWeapon(TSubclassOf<UWeaponDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable)
	UWeaponInstance* GetEquippedWeaponInstance();

	UFTAAbilitySystemComponent* GetAbilitySystemComponent() const;
};
