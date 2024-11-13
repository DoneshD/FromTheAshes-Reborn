#include "Weapons/EquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "Weapons/FTAEquipmentDefinition.h"
#include "Weapons/FTAEquipmentInstance.h"


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

UFTAEquipmentInstance* FFTAEquipmentList::AddEntry(TSubclassOf<UFTAEquipmentDefinition> EquipmentDefinition)
{
	UFTAEquipmentInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());
	
	const UFTAEquipmentDefinition* EquipmentCDO = GetDefault<UFTAEquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<UFTAEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UFTAEquipmentInstance::StaticClass();
	}
	
	FFTAAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UFTAEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	Result = NewEntry.Instance;

	if (UFTAAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (TObjectPtr<const UFTAAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			// AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FFTAEquipmentList::AddEntry - ASC IS NULL"));
	}

	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);
	
	return Result;
}

void FFTAEquipmentList::RemoveEntry(UFTAEquipmentInstance* Instance)
{
	
}

UFTAAbilitySystemComponent* FFTAEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	return Cast<UFTAAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}