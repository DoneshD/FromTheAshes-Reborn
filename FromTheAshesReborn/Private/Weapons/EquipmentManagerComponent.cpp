#include "Weapons/EquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "Weapon/WeaponDefinition.h"
#include "Weapon/WeaponInstance.h"
#include "Weapon/WeaponInstance.h"
#include "Weapon/WeaponDefinition.h"


UEquipmentManagerComponent::UEquipmentManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UEquipmentManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
											   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}



FString FFTAAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

UWeaponInstance* UEquipmentManagerComponent::AddEntry(TSubclassOf<UWeaponDefinition> EquipmentDefinition)
{
	UWeaponInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
	// check(OwnerComponent);
	// check(OwnerComponent->GetOwner()->HasAuthority());
	
	const UWeaponDefinition* EquipmentCDO = GetDefault<UWeaponDefinition>(EquipmentDefinition);

	TSubclassOf<UWeaponInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UWeaponInstance::StaticClass();
	}
	
	FFTAAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UWeaponInstance>(GetOwner(), InstanceType);
	Result = NewEntry.Instance;

	if (UFTAAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (TObjectPtr<const UFTAAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FFTAEquipmentList::AddEntry - ASC IS NULL"));
	}

	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);
	
	return Result;
}

void UEquipmentManagerComponent::RemoveEntry(UWeaponInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFTAAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			if (UFTAAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			Instance->DestroyEquipmentActors();
			EntryIt.RemoveCurrent();
		}
	}
}


UFTAAbilitySystemComponent* UEquipmentManagerComponent::GetAbilitySystemComponent() const
{
	// check(OwnerComponent);
	// AActor* OwningActor = OwnerComponent->GetOwner();
	return Cast<UFTAAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}