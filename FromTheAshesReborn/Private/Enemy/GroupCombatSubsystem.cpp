#include "Enemy/GroupCombatSubsystem.h"

#include "CombatComponents/GroupCombatComponent.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void UGroupCombatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UGroupCombatSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
}

void UGroupCombatSubsystem::RegisterEnemyToGroupCombat(TObjectPtr<AEnemyBaseCharacter> Enemy)
{
	AllEnemiesArray.Add(Enemy);
}

