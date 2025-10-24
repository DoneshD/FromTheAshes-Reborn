#include "Enemy/GroupCombatSubsystem.h"



void UGroupCombatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
}

void UGroupCombatSubsystem::Deinitialize()
{
	Super::Deinitialize();

	UE_LOG(LogTemp, Warning, TEXT("Instance Init num: %d"), AllEnemiesArray.Num());
	
}

void UGroupCombatSubsystem::RegisterEnemyToGroupCombat(TObjectPtr<AEnemyBaseCharacter> Actor)
{
	AllEnemiesArray.Add(Actor);
}
