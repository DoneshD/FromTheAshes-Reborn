#pragma once

#include "CoreMinimal.h"
#include "FTAAbilityDataAsset.h"
#include "AttackAbilityDataAsset.generated.h"

struct FHitDataInfo;
class UWeaponCueObject;
class AWeaponActorBase;
class UFTAVisualCueObject;
class UFTASoundCueObject;
class UHitCueObject;
class UGA_ReceiveHit;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAttackDirectionStruct
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bEnabled;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ESpatialDirection AttackDirection;

	FAttackDirectionStruct()
	:
	bEnabled(false),
	AttackDirection(ESpatialDirection::None)
	{}
	
	
};

USTRUCT(BlueprintType)
struct FAttackDataStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TSubclassOf<UGameplayEffect> ApplyDamageEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TArray<FHitDataInfo> PossibleHitReactions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cue Data")
	TArray<TSubclassOf<UWeaponCueObject>> MeleeTrailCueClassArray;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cue Data")
	TArray<TSubclassOf<UFTAVisualCueObject>> HitEnemyVisualCueClassArray;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cue Data")
	TArray<TSubclassOf<UFTASoundCueObject>> HitEnemySoundCueClassArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cue Data")
	TArray<TSubclassOf<UWeaponCueObject>> HitEnvironmentVisualCueClassArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TSubclassOf<AWeaponActorBase> WeaponActorClass; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	FAttackDirectionStruct AttackDirectionStruct;
	
};

UCLASS()
class FROMTHEASHESREBORN_API UAttackAbilityDataAsset : public UFTAAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FAttackDataStruct AttackData;

};
