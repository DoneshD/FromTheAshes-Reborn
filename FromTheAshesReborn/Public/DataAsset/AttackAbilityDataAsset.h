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
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack | Damage")
	TSubclassOf<UGameplayEffect> ApplyDamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack | Hit | Reactions")
	TArray<FHitDataInfo> PossibleHitReactions;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attack | Hit | Movement")
	bool SupplyMovementDataOnHit = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attack | Hit | Movement")
	bool FollowEndLocationOnHit = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack | Hit | Movement")
	FVector RelativeOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack | Hit | HitStop")
	FGameplayTag HitStopCueTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack | Hit | HitStop")
	FGameplayTag CameraShakeCueTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack | Hit | Effects")
	TArray<TSubclassOf<UFTAVisualCueObject>> HitEnemyVisualCueClassArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack | Hit | Effects")
	TArray<TSubclassOf<UFTASoundCueObject>> HitEnemySoundCueClassArray;


};
