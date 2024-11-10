#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FTAEquipmentDefinition.generated.h"

class AActor;
class UFTAAbilitySet;
class UFTAEquipmentInstance;

USTRUCT()
struct FFTAEquipmentActorToSpawn
{
	GENERATED_BODY()

	FFTAEquipmentActorToSpawn()
	{}

	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform;
};

UCLASS()
class FROMTHEASHESREBORN_API UFTAEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:

	UFTAEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Class to spawn
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TSubclassOf<UFTAEquipmentInstance> InstanceType;

	// Gameplay ability sets to grant when this is equipped
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<TObjectPtr<const UFTAAbilitySet>> AbilitySetsToGrant;

	// Actors to spawn on the pawn when this is equipped
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<FFTAEquipmentActorToSpawn> ActorsToSpawn;
};
