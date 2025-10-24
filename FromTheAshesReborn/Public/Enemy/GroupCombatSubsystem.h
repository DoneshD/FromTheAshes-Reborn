#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GroupCombatSubsystem.generated.h"

class AEnemyBaseCharacter;

UCLASS()
class FROMTHEASHESREBORN_API UGroupCombatSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	
	UPROPERTY()
	TArray<AEnemyBaseCharacter*> AllEnemiesArray;

public:

	int32 AttackTokensCount;

protected:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:

	void RegisterEnemyToGroupCombat(TObjectPtr<AEnemyBaseCharacter> Actor);
	
};
