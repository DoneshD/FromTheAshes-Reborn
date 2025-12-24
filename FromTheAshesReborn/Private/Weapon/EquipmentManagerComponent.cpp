#include "Weapon/EquipmentManagerComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "Weapon/WeaponDefinition.h"
#include "Weapon/WeaponInstance.h"
#include "Weapon/WeaponActorBase.h"


UEquipmentManagerComponent::UEquipmentManagerComponent(): CurrentEquippedWeaponInstance(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UEquipmentManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

FString FFTAAppliedEquipmentItem::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
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

UWeaponInstance* UEquipmentManagerComponent::SetEquippedWeapon(TSubclassOf<UWeaponDefinition> WeaponDefinition)
{
	if(CurrentEquippedWeaponInstance)
	{
		RemoveEquipmentItem(CurrentEquippedWeaponInstance);
	}
	
	check(WeaponDefinition != nullptr);
	
	const UWeaponDefinition* WeaponCDO = GetDefault<UWeaponDefinition>(WeaponDefinition);

	TSubclassOf<UWeaponInstance> InstanceType = WeaponCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UWeaponInstance::StaticClass();
	}

	CurrentType = WeaponCDO->WeaponType;
	
	FFTAAppliedEquipmentItem& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = WeaponDefinition;
	NewEntry.Instance = NewObject<UWeaponInstance>(GetOwner(), InstanceType);
	UWeaponInstance* Result = NewEntry.Instance;

	if (UFTAAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (TObjectPtr<const UFTAAbilitySet> AbilitySet : WeaponCDO->AbilitySetsToGrant)
		{
			if (AbilitySet)
			{
				AbilitySet->GiveToAbilitySystem(ASC, &NewEntry.GrantedHandles, Result);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FFTAEquipmentList::AddEntry - ASC IS NULL"));
	}

	Result->SpawnEquipmentActors(WeaponCDO->ActorsToSpawn);
	Result->OnEquipped();
	CurrentEquippedWeaponInstance = Result;

	for (AActor* SpawnedActor : CurrentEquippedWeaponInstance->GetSpawnedActors())
	{
		if(AWeaponActorBase* WeaponActor = Cast<AWeaponActorBase>(SpawnedActor))
		{
			CurrentEquippedWeaponActor = WeaponActor;
		}
	}
	
	return Result;
}

UWeaponInstance* UEquipmentManagerComponent::GetEquippedWeaponInstance()
{
	if(CurrentEquippedWeaponInstance)
	{
		return CurrentEquippedWeaponInstance;
	}
	UE_LOG(LogTemp, Error, TEXT("No currently equipped instance"));
	return nullptr;
}

AWeaponActorBase* UEquipmentManagerComponent::GetEquippedWeaponActor()
{
	if(CurrentEquippedWeaponActor)
	{
		return CurrentEquippedWeaponActor;
	}
	UE_LOG(LogTemp, Error, TEXT("No currently equipped actor"));
	return nullptr;
}


UFTAAbilitySystemComponent* UEquipmentManagerComponent::GetAbilitySystemComponent() const
{
	AFTACharacter* FTACharacter = Cast<AFTACharacter>(GetOwner());
	return FTACharacter->GetFTAAbilitySystemComponent();
}

bool UEquipmentManagerComponent::AddWeaponDefinitionToEquipment(TSubclassOf<UWeaponDefinition> EquipmentDefinition)
{
	AllWeaponDefinitions.Add(EquipmentDefinition);

	const UWeaponDefinition* WeaponCDO = GetDefault<UWeaponDefinition>(EquipmentDefinition);

	if(WeaponCDO->WeaponType.MatchesTagExact(FGameplayTag::RequestGameplayTag("Ascendant")))
	{
		AscendantWeaponDefinitions.Add(EquipmentDefinition);
	}
	else if(WeaponCDO->WeaponType.MatchesTagExact(FGameplayTag::RequestGameplayTag("Abyssal")))
	{
		AbyssalWeaponDefinitions.Add(EquipmentDefinition);
	}
	else if(WeaponCDO->WeaponType.MatchesTagExact(FGameplayTag::RequestGameplayTag("Zenith")))
	{
		ZenithWeaponDefinitions.Add(EquipmentDefinition);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipmentManagerComponent::AddWeaponDefinitionToEquipment - Matches None"));
	}
	
	return true;
}
