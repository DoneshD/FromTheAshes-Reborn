#include "GameModes/FTAGameModeBase.h"

#include "CombatComponents/GroupCombatComponent.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "Enemy/GroupCombatSubsystem.h"
#include "Kismet/KismetMathLibrary.h"

void AFTAGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	// UGroupCombatSubsystem* GCS = GetWorld()->GetSubsystem<UGroupCombatSubsystem>();
	//
	// if(!GCS || !GCS->IsValidLowLevel())
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("AFTAGameModeBase::BeginPlay() - GCS is Null"))
	// 	return;
	// }
	//
	// int32 RandomIndex = UKismetMathLibrary::RandomInteger(GCS->AllEnemiesArray.Num());
	//
	// bool bValidIndex = GCS->AllEnemiesArray.IsValidIndex(RandomIndex);
	//
	// if(!bValidIndex)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("AFTAGameModeBase::BeginPlay() - Invalid index"));
	// 	return;
	// }
	//
	// AEnemyBaseCharacter* Enemy = GCS->AllEnemiesArray[RandomIndex];
	//
	// if(!Enemy || !Enemy->IsValidLowLevel())
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("AFTAGameModeBase::BeginPlay() - Enemy is Null"))
	// 	return;
	// }
	//
	// UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();
	//
	// if(!GCC || !GCC->IsValidLowLevel())
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("AFTAGameModeBase::BeginPlay() - GCC is Null"))
	// 	return;
	// }
	//
	// GCC->AttackTokensCount += 1;
	
}
