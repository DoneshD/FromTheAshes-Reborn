#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyEncounterDataAsset.generated.h"

class UEnemyCharacterDataAsset;

USTRUCT(BlueprintType)
struct FPlayerTokenData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 MinimumCount = 0;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 StartingCount = 0;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 MaximumCount = 0;
	
};

USTRUCT(BlueprintType)
struct FRoleData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 MinimumRoleCount = 0;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 StartingRoleCount = 0;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 MaximumRoleCount = 0;
	
};

USTRUCT(BlueprintType)
struct FWaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<UEnemyCharacterDataAsset*, int32> EnemySpawnCount;
	
};

UCLASS()
class FROMTHEASHESREBORN_API UEnemyEncounterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Wave")
	FWaveData WaveData;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Roles")
	FRoleData AggressorRoles;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Roles")
	FRoleData CoverRoles;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Roles")
	FRoleData ObserverRoles;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Player")
	FPlayerTokenData PlayerTokenData;
	
};
