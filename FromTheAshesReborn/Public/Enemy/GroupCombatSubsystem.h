#pragma once

#include "CoreMinimal.h"
#include "EnemyBaseCharacter.h"
#include "DataAsset/EnemyEncounterDataAsset.h"
#include "Subsystems/WorldSubsystem.h"
#include "GroupCombatSubsystem.generated.h"

class AAIControllerEnemyBase;
class AWaveManager;
class AEnemyBaseCharacter;

struct FRoleRequirement
{
	EEnemyEngagementRole Role;
	int32 MinCount;
	int32 MaxCount;
	int32 CurrentCount = 0;
};

UCLASS()
class FROMTHEASHESREBORN_API UGroupCombatSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	TObjectPtr<AWaveManager> WaveManager;

	bool Flip = true;
	FTimerHandle AddTimer;

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
	void AssignInitialRoles(EEnemyEngagementRole Role, int32 StartingRoleCount);
	
	void AssignInitialWeightedRandomEngagementRole(EEnemyEngagementRole InRole, TArray<AEnemyBaseCharacter*> InEnemies);
	void AssignWeightedRandomEngagementRole(EEnemyEngagementRole InRole, TArray<AEnemyBaseCharacter*> InEnemies);

	AEnemyBaseCharacter* SelectWeightedRandomEnemy(EEnemyEngagementRole InRole, TArray<AEnemyBaseCharacter*>& InEnemies);

	TArray<AEnemyBaseCharacter*> GetAllAvailableEnemies();

	void AssignEngagementRole(TObjectPtr<AEnemyBaseCharacter>, EEnemyEngagementRole InRole);
	
	void StartAttacking(TObjectPtr<AAIControllerEnemyBase> InEnemyController);

	void EnforceAllEngagementRoleCounts();
	void GetRoleCountsFromValidEnemies(TArray<FRoleRequirement>& OutRoleRequirements, TArray<AEnemyBaseCharacter*>& OutValidEnemies);
	void SatisfyMinimumRoleCounts(TArray<FRoleRequirement>& InRequirements, TArray<AEnemyBaseCharacter*>& InValidEnemies);
	void SatisfyMaximumRoleCounts(TArray<FRoleRequirement>& InRequirements, TArray<AEnemyBaseCharacter*>& InValidEnemies);
	
	void SwapOutAggressor(TObjectPtr<AEnemyBaseCharacter> InEnemy);
	
	void ActivateAllStateTrees();

	int32 GetNumInRoles(EEnemyEngagementRole InRole);

	void ResetTimeSpentInRole(TObjectPtr<AEnemyBaseCharacter> Enemy);

	void FlipAggressionCount();
	
	void AddRole(EEnemyEngagementRole InRole);
	
};
