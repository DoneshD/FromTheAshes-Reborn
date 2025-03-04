#include "Weapon/WeaponInstance.h"

#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/Character.h"
#include "Weapon/WeaponActorBase.h"
#include "Weapon/WeaponDefinition.h"

UWeaponInstance::UWeaponInstance(const FObjectInitializer& ObjectInitializer)
{
}

APawn* UWeaponInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* UWeaponInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if (UClass* ActualPawnType = PawnType)
	{
		if (GetOuter()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

void UWeaponInstance::SpawnEquipmentActors(const TArray<FEquipmentActorToSpawn>& ActorsToSpawn)
{
	if (APawn* OwningPawn = GetPawn())
	{
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			AttachTarget = Char->GetMesh();
		}

		for (const FEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);
			//DEBUG REMOVE LATER

			AFTACharacter* AFTAChar = Cast<AFTACharacter>(GetPawn());
			
			UFTAAbilitySystemComponent* FTAASC = AFTAChar->GetFTAAbilitySystemComponent();
			FTAASC->TestWeaponActor = Cast<AWeaponActorBase>(NewActor);
			SpawnedActors.Add(NewActor);
		}
	}
}

void UWeaponInstance::DestroyEquipmentActors()
{
	for(AActor* Actor : SpawnedActors)
	{
		if(Actor)
		{
			Actor->Destroy();
		}
	}
}

void UWeaponInstance::OnEquipped()
{
	ACharacter* Character = Cast<ACharacter>(GetPawn());
	if(!Character || !EquippedAnimSet.DefaultLayer)
	{
		UE_LOG(LogTemp, Error, TEXT("Character is NULL"))
	}
	Character->GetMesh()->LinkAnimClassLayers(EquippedAnimSet.DefaultLayer);
	K2_OnEquipped();
}

void UWeaponInstance::OnUnEquipped()
{
	K2_OnUnEquipped();
}

TSubclassOf<UAnimInstance> UWeaponInstance::PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const
{
	const FAnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UnEquippedAnimSet);
	return SetToQuery.SelectBestLayer(CosmeticTags);
}