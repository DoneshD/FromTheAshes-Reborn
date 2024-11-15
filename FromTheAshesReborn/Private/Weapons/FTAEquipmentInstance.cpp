
#include "Weapons/FTAEquipmentInstance.h"
#include "GameFramework/Character.h"
#include "Weapons/FTAEquipmentDefinition.h"

UFTAEquipmentInstance::UFTAEquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

APawn* UFTAEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* UFTAEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
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

void UFTAEquipmentInstance::SpawnEquipmentActors(const TArray<FFTAEquipmentActorToSpawn>& ActorsToSpawn)
{
	if (APawn* OwningPawn = GetPawn())
	{
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			AttachTarget = Char->GetMesh();
		}

		for (const FFTAEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

			SpawnedActors.Add(NewActor);
		}
	}
}

void UFTAEquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

void UFTAEquipmentInstance::OnEquipped()
{
	UE_LOG(LogTemp, Warning, TEXT("Need to implement UFTAEquipmentInstance::OnEquipped()"));
}

void UFTAEquipmentInstance::OnUnequipped()
{
	UE_LOG(LogTemp, Warning, TEXT("Need to implement UFTAEquipmentInstance::OnUnequipped()"));
}
