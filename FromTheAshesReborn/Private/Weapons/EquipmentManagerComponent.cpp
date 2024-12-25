#include "Weapons/EquipmentManagerComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "Player/FTAPlayerController.h"
#include "Weapon/WeaponDefinition.h"
#include "Weapon/WeaponInstance.h"


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



FString FFTAAppliedEquipmentItem::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

UWeaponInstance* UEquipmentManagerComponent::AddEquipmentItem(TSubclassOf<UWeaponDefinition> EquipmentDefinition)
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
	
	FFTAAppliedEquipmentItem& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UWeaponInstance>(GetOwner(), InstanceType);
	Result = NewEntry.Instance;

	if (UFTAAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (TObjectPtr<const UFTAAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			if (AbilitySet)
			{
				AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FFTAEquipmentList::AddEntry - ASC IS NULL"));
	}

	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);
	
	return Result;
}

void UEquipmentManagerComponent::RemoveEquipmentItem(UWeaponInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FFTAAppliedEquipmentItem& Entry = *EntryIt;
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
	//TODO: NEED TO FIND A WAY ON HOW TO IVE SAME ASC TO PLAYER STATE AND CHARACTER!!!!!
	AFTAPlayerController* PC = Cast<AFTAPlayerController>(GetOwner()->GetInstigatorController());
	return PC->GetFTAAbilitySystemComponent();
}