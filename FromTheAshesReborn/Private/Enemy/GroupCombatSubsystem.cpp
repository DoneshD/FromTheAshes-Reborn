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

void UGroupCombatSubsystem::GrantRandomInitialAttackTokens()
{
	int32 RandomIndex = UKismetMathLibrary::RandomInteger(AllEnemiesArray.Num());
	
	bool bValidIndex = AllEnemiesArray.IsValidIndex(RandomIndex);
	
	if(!bValidIndex)
	{
		UE_LOG(LogTemp, Error, TEXT("AFTAGameModeBase::BeginPlay() - Invalid index"));
		return;
	}
	
	AEnemyBaseCharacter* Enemy = AllEnemiesArray[RandomIndex];
	
	if(!Enemy || !Enemy->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("AFTAGameModeBase::BeginPlay() - Enemy is Null"))
		return;
	}
	
	UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();
	
	if(!GCC || !GCC->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("AFTAGameModeBase::BeginPlay() - GCC is Null"))
		return;
	}
	
	GCC->AttackTokensCount += 1;
}

