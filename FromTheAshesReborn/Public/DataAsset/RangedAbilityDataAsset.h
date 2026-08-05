#pragma once

#include "CoreMinimal.h"
#include "AttackAbilityDataAsset.h"
#include "RangedAbilityDataAsset.generated.h"


class AFTAProjectile;

UENUM(BlueprintType)
enum class EHand : uint8
{
	None	UMETA(DisplayName = "None"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
};

UENUM(BlueprintType)
enum class ETrajectoryRoute : uint8
{
	None	UMETA(DisplayName = "None"),
	CenterScreen	UMETA(DisplayName = "CenterScreen"),
	Target	UMETA(DisplayName = "Target"),
	ActorAngle	UMETA(DisplayName = "ActorAngle")
};

UCLASS()
class FROMTHEASHESREBORN_API URangedAbilityDataAsset : public UAttackAbilityDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Ranged")
	ETrajectoryRoute TrajectoryRoute = ETrajectoryRoute::CenterScreen;

	UPROPERTY(EditDefaultsOnly, Category = "Ranged")
	bool SpawnProjectileActor = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ranged", meta = (EditCondition = "SpawnProjectileActor"))
	TSubclassOf<AFTAProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Ranged")
	EHand Hand;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	TArray<TSubclassOf<UWeaponCueObject>> MuzzleVisualCueArray;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	TArray<TSubclassOf<UFTASoundCueObject>> MuzzleSoundCueClassArray;
	
};
