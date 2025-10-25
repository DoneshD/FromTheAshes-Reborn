#include "Enemy/GroupCombatSubsystem.h"

#include "CombatComponents/GroupCombatComponent.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void UGroupCombatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Warning, TEXT("UGroupCombatSubsystem::Initialize"));
}

void UGroupCombatSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
}

void UGroupCombatSubsystem::RegisterEnemyToGroupCombat(TObjectPtr<AEnemyBaseCharacter> Actor)
{
	AllEnemiesArray.Add(Actor);
}

