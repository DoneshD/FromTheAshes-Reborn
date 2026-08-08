#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GroupCombatSubsystem.generated.h"

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

public:
	
	UPROPERTY()
	TArray<AEnemyBaseCharacter*> AllEnemiesArray;

	UPROPERTY()
	TMap<TObjectPtr<AEnemyBaseCharacter>, int32> EnemiesAttackTokensMap;

protected:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	
	void RegisterEnemyToGroupCombat(TObjectPtr<AEnemyBaseCharacter> Actor);

	void SwapOutAggressor(TObjectPtr<AEnemyBaseCharacter> InEnemy);
	
	int32 GetAggressorCount() const;

	void ActivateAllStateTrees();

	void PrintAllAttackTokens();
};
