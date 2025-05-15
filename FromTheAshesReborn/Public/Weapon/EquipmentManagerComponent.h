
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FTAAbilitySystem/AbilitySets/FTAAbilitySet.h"
#include "EquipmentManagerComponent.generated.h"

class AWeaponActorBase;
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

	UPROPERTY()
	TSubclassOf<UWeaponDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UWeaponInstance> Instance = nullptr;

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

	UPROPERTY()
	UWeaponInstance* CurrentEquippedWeaponInstance;

	UPROPERTY()
	TObjectPtr<AWeaponActorBase> CurrentEquippedWeaponActor;

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

	UFUNCTION(BlueprintCallable)
	AWeaponActorBase* GetEquippedWeaponActor();

	UFTAAbilitySystemComponent* GetAbilitySystemComponent() const;
};
