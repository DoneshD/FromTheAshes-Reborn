#pragma once

#include "CoreMinimal.h"
#include "EnemyBaseCharacter.h"
#include "Subsystems/WorldSubsystem.h"
#include "GroupCombatSubsystem.generated.h"

class AAIControllerEnemyBase;
class AWaveManager;
class AEnemyBaseCharacter;


USTRUCT(BlueprintType)
struct FRoleRequirement
{

	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	EEnemyEngagementRole Role;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 MinCount;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 MaxCount;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 StartingCount;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 CurrentCount;
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

	TArray<FRoleRequirement> AllRoleRequirements;

protected:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	
	void RegisterAllEnemiesToGroupCombat();
	void AssignInitialRoles(EEnemyEngagementRole Role, int32 StartingRoleCount);
	
	void AssignInitialWeightedRandomEngagementRole(EEnemyEngagementRole InRole, TArray<AEnemyBaseCharacter*> InEnemies);
	void AssignWeightedRandomEngagementRole(EEnemyEngagementRole InRole, TArray<AEnemyBaseCharacter*> InEnemies);

	AEnemyBaseCharacter* SelectWeightedRandomEnemy(EEnemyEngagementRole InRole, TArray<AEnemyBaseCharacter*>& InEnemies);
	AEnemyBaseCharacter* SelectWeightRandomEnemyFromAllRoles();

	float CalculateDistanceScore(TObjectPtr<AEnemyBaseCharacter> InEnemy);
	float CalculateRoleBias(AEnemyBaseCharacter* InEnemy);

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
	void ChangeMinMaxRoleCount(EEnemyEngagementRole InRole, int32 InMinCount, int32 InMaxCount);
	
	void AddRole(EEnemyEngagementRole InRole);

	
	
};
