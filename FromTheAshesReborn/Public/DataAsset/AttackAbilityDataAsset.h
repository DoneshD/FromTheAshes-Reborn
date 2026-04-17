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


UCLASS()
class FROMTHEASHESREBORN_API UAttackAbilityDataAsset : public UFTAAbilityDataAsset
{
	GENERATED_BODY()

public:
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<UGameplayEffect> ApplyDamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	TArray<FHitDataInfo> PossibleHitReactions;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attack Data")
	bool SupplyMovementDataOnHit = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attack Data")
	bool FollowEndLocationOnHit = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	TArray<TSubclassOf<UFTAVisualCueObject>> HitEnemyVisualCueClassArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	TArray<TSubclassOf<UFTASoundCueObject>> HitEnemySoundCueClassArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector RelativeOffset = FVector::ZeroVector;

};
