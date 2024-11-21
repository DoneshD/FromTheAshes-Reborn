
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FTAAbilitySystem/AbilitySets/FTAAbilitySet.h"
#include "EquipmentManagerComponent.generated.h"

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
	friend FFTAEquipmentList;
	friend UEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UFTAEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UFTAEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	FFTAAbilitySet_GrantedHandles GrantedHandles;
};


USTRUCT(BlueprintType)
struct FFTAEquipmentList
{
	GENERATED_BODY()

	FFTAEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FFTAEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	
	UFTAEquipmentInstance* AddEntry(TSubclassOf<UFTAEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UFTAEquipmentInstance* Instance);

private:
	UFTAAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UEquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FFTAAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};






UCLASS(BlueprintType, Const)
class FROMTHEASHESREBORN_API UEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentManagerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
