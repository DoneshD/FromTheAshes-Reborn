#pragma once

#include "CoreMinimal.h"
#include "EnemyBaseCharacter.h"
#include "DataAsset/EnemyEncounterDataAsset.h"
#include "Subsystems/WorldSubsystem.h"
#include "GroupCombatSubsystem.generated.h"

class AWaveManager;
class AEnemyBaseCharacter;

/*UENUM(BlueprintType)
enum class EEnemyEngagementRole : uint8
{
	None		UMETA(DisplayName = "None"),
	Aggressor	UMETA(DisplayName = "Aggressor"),
	Cover		UMETA(DisplayName = "Cover"),
	Observer	UMETA(DisplayName = "Observer")
};*/

UCLASS()
class FROMTHEASHESREBORN_API UGroupCombatSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	TObjectPtr<AWaveManager> WaveManager;

	bool Flip = true;

public:
	
	UPROPERTY()
	TArray<AEnemyBaseCharacter*> AllEnemiesArray;

	UPROPERTY()
	TMap<TObjectPtr<AEnemyBaseCharacter>, int32> EnemiesAttackTokensMap;

	FTimerHandle AggressionTimer;

protected:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	
	void RegisterAllEnemiesToGroupCombat();
	void AssignInitialRoles(EEnemyEngagementRole Role, int32 StartingRoleCount);
	
	void AssignInitialWeightedRandomEngagementRole(EEnemyEngagementRole InRole, TArray<AEnemyBaseCharacter*> InEnemies);
	void AssignWeightedRandomEngagementRole(EEnemyEngagementRole InRole, TArray<AEnemyBaseCharacter*> InEnemies);

	AEnemyBaseCharacter* SelectWeightedRandomEnemy(EEnemyEngagementRole InRole, TArray<AEnemyBaseCharacter*>& InEnemies);

	TArray<AEnemyBaseCharacter*> GetAllAvailableEnemies();

	void AssignEngagementRole(TObjectPtr<AEnemyBaseCharacter>, EEnemyEngagementRole InRole);

	void EnforceAllEngagementRoleCounts();
	
	void SwapOutAggressor(TObjectPtr<AEnemyBaseCharacter> InEnemy);
	
	void ActivateAllStateTrees();

	int32 GetNumOfRoles(EEnemyEngagementRole InRole);

	void ResetTimeSpentInRole(TObjectPtr<AEnemyBaseCharacter> Enemy);

	void FlipAggression();
	
};
