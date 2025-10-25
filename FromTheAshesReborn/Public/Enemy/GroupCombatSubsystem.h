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

	int32 AttackTokensCount;

	UPROPERTY()
	TArray<AEnemyBaseCharacter*> AllEnemiesArray;

protected:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:

	void RegisterEnemyToGroupCombat(TObjectPtr<AEnemyBaseCharacter> Actor);
};
