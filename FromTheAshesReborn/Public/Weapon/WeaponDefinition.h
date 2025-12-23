#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "WeaponDefinition.generated.h"

class AWeaponActorBase;
class AActor;
class UFTAAbilitySet;
class UWeaponInstance;

USTRUCT()
struct FEquipmentActorToSpawn
{
	GENERATED_BODY()

	FEquipmentActorToSpawn()
	{}

	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform;
};

UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class FROMTHEASHESREBORN_API UWeaponDefinition : public UObject
{
	GENERATED_BODY()

public:

	UWeaponDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TSubclassOf<UWeaponInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<TObjectPtr<const UFTAAbilitySet>> AbilitySetsToGrant;

	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<FEquipmentActorToSpawn> ActorsToSpawn;

	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	FGameplayTag WeaponType = FGameplayTag::EmptyTag;

	
};