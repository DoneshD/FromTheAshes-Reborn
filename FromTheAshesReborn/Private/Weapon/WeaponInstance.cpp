#include "Weapon/WeaponInstance.h"
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
			
			SpawnedActors.Add(NewActor);
		}
	}
}

void UWeaponInstance::DestroyEquipmentActors()
{
}

void UWeaponInstance::OnEquipped()
{
}

void UWeaponInstance::OnUnequipped()
{
}

TSubclassOf<UAnimInstance> UWeaponInstance::PickBestAnimLayer(bool bEquipped,
	const FGameplayTagContainer& CosmeticTags) const
{
	const FFTAAnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UnEquippedAnimSet);
	return SetToQuery.SelectBestLayer(CosmeticTags);
}
