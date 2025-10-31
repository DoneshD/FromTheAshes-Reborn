#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GroupCombatSubsystem.generated.h"

class AEnemyBaseCharacter;

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
};
