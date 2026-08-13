#pragma once

#include "CoreMinimal.h"
#include "DataAsset/EnemyEncounterDataAsset.h"
#include "Subsystems/WorldSubsystem.h"
#include "GroupCombatSubsystem.generated.h"

class AWaveManager;
class AEnemyBaseCharacter;

UENUM(BlueprintType)
enum class EEnemyEngagementRole : uint8
{
	None		UMETA(DisplayName = "None"),
	Aggressor	UMETA(DisplayName = "Aggressor"),
	Cover		UMETA(DisplayName = "Cover"),
	Observer	UMETA(DisplayName = "Observer")
};

UCLASS()
class FROMTHEASHESREBORN_API UGroupCombatSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	TObjectPtr<AWaveManager> WaveManager;

public:
	
	UPROPERTY()
	TArray<AEnemyBaseCharacter*> AllEnemiesArray;

	UPROPERTY()
	TMap<TObjectPtr<AEnemyBaseCharacter>, int32> EnemiesAttackTokensMap;

protected:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	
	void RegisterAllEnemiesToGroupCombat();
	void AssignEngagementRole(UEnemyEncounterDataAsset* InEncounterData, EEnemyEngagementRole InRole);

	void EnforceEngagementRoleCount(UEnemyEncounterDataAsset* InEncounterData, EEnemyEngagementRole InRole);
	
	void SwapOutAggressor(TObjectPtr<AEnemyBaseCharacter> InEnemy);
	void ActivateAllStateTrees();

	int32 GetNumOfRoles(EEnemyEngagementRole InRole);
	
};
