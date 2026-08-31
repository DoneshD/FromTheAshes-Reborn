#include "Enemy/GroupCombatSubsystem.h"

#include "AbilitySystemComponent.h"
#include "StateTreeEvents.h"
#include "Algo/RandomShuffle.h"
#include "CombatComponents/GroupCombatComponent.h"
#include "DataAsset/AICombatParameters.h"
#include "DataAsset/EnemyEncounterDataAsset.h"
#include "Enemy/AIControllerEnemyBase.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "Enemy/FTAStateTreeAIComponent.h"
#include "GameModes/FTAGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Level/WaveManager.h"
#include "Player/PlayerCharacter.h"

void UGroupCombatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	/*WaveManager = Cast<AWaveManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWaveManager::StaticClass()));
	if(!WaveManager)
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager is NULL"));
		return;
	}
	WaveManager->OnAllEnemiesSpawnedInWave.AddUObject(this, &UGroupCombatSubsystem::RegisterAllEnemiesToGroupCombat);*/
}

void UGroupCombatSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
}

void UGroupCombatSubsystem::RegisterAllEnemiesToGroupCombat()
{

	TArray<AActor*> Enemies;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBaseCharacter::StaticClass(),Enemies);
	
	for (AActor* Actor : Enemies)
	{
		AEnemyBaseCharacter* Enemy = Cast<AEnemyBaseCharacter>(Actor);
		{
			AllEnemiesArray.Add(Enemy);
		}
	}

	AFTAGameModeBase* FTAGameMode = Cast<AFTAGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if(!FTAGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid gamemode"));
		return;
	}
	
	UEnemyEncounterDataAsset* EncounterData = FTAGameMode->EnemyEncounterArray[FTAGameMode->CurrentEncounter];

	// AssignAllRandomEngagementRole(EncounterData, EEnemyEngagementRole::Aggressor);
	// AssignAllRandomEngagementRole(EncounterData, EEnemyEngagementRole::Cover);
	// AssignAllRandomEngagementRole(EncounterData, EEnemyEngagementRole::Observer);
	

	for(int i = 0; i < EncounterData->AggressorRoles.StartingRoleCount; i++)
	{
		auto Enemy = GetAllAvailableEnemies()[FMath::RandRange(0, GetAllAvailableEnemies().Num() - 1)];
		if(Enemy)
		{
			UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();
			if(GCC)
			{
				if(Enemy->AICombatParams->AggressionLevel == 1.0f)
				{
					UE_LOG(LogTemp, Warning, TEXT("1.0f"))
					GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();
					GCC->EngagementRole = EEnemyEngagementRole::Aggressor;
					ResetTimeSpentInRole(Enemy);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Else"))
					
					AssignAllWeightedRandomSelectionEngagementRole(EEnemyEngagementRole::Aggressor, GetAllAvailableEnemies());
				}
			}
		}
		
		
	}
	
	for(int i = 0; i < EncounterData->CoverRoles.StartingRoleCount; i++)
	{
		auto Enemy = GetAllAvailableEnemies()[FMath::RandRange(0, GetAllAvailableEnemies().Num() - 1)];
		
		if(Enemy)
		{
			UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();
			if(GCC)
			{
				if(Enemy->AICombatParams->CoverLevel == 1.0f)
				{
					GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();
					GCC->EngagementRole = EEnemyEngagementRole::Cover;
					ResetTimeSpentInRole(Enemy);
				}
				else
				{
					AssignAllWeightedRandomSelectionEngagementRole(EEnemyEngagementRole::Cover, GetAllAvailableEnemies());
				}
			}
		}
		
	}
	
	for(int i = 0; i < EncounterData->ObserverRoles.StartingRoleCount; i++)
	{
		auto Enemy = GetAllAvailableEnemies()[FMath::RandRange(0, GetAllAvailableEnemies().Num() - 1)];
		
		if(Enemy)
		{
			UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();
			if(GCC)
			{
				if(Enemy->AICombatParams->ObserverLevel == 1.0f)
				{
					GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();
					GCC->EngagementRole = EEnemyEngagementRole::Observer;
					ResetTimeSpentInRole(Enemy);
				}
				else
				{
					AssignAllWeightedRandomSelectionEngagementRole(EEnemyEngagementRole::Observer, GetAllAvailableEnemies());
				}
			}
		}
		
	}

	for (AEnemyBaseCharacter* Enemy : AllEnemiesArray)
	{
		if (Enemy)
		{
			Enemy->OnPlayerInAggressionRadius.AddUObject(this, &UGroupCombatSubsystem::AssignEngagementRole);
			
			UAbilitySystemComponent* ASC = Enemy->FindComponentByClass<UAbilitySystemComponent>();
	
			ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("HitTag.State.Hit"), EGameplayTagEventType::NewOrRemoved).AddLambda(
					[this, Enemy](const FGameplayTag Tag, int32 NewCount)
					{
						if (NewCount > 0)
						{
							SwapOutAggressor(Enemy);
						}
					}
				);
		}
	}
	
	// GetWorld()->GetTimerManager().SetTimer(
	// 	AggressionTimer,
	// 	this,
	// 	&UGroupCombatSubsystem::FlipAggression,
	// 	10.0f,
	// 	true
	// );
}

void UGroupCombatSubsystem::AssignAllRandomEngagementRole(UEnemyEncounterDataAsset* InEncounterData, EEnemyEngagementRole InRole)
{
	int32 RoleCount;

	if(InRole == EEnemyEngagementRole::Aggressor)
	{
		RoleCount = InEncounterData->AggressorRoles.StartingRoleCount;
	}
	else if(InRole == EEnemyEngagementRole::Cover)
	{
		RoleCount = InEncounterData->CoverRoles.StartingRoleCount;
	}
	else if(InRole == EEnemyEngagementRole::Observer)
	{
		RoleCount = InEncounterData->ObserverRoles.StartingRoleCount;
	}
	else
	{
		RoleCount = 0;
		UE_LOG(LogTemp, Error, TEXT("UGroupCombatSubsystem::AssignEngagementRole - Invalid Role"))
		return;
	}
	
	for (int32 i = 0; i < RoleCount; i++)
	{
		TArray<AEnemyBaseCharacter*> AvailableEnemies;

		for (AEnemyBaseCharacter* Enemy : AllEnemiesArray)
		{
			if (!Enemy)
			{
				continue;
			}

			UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();

			if (GCC && GCC->EngagementRole == EEnemyEngagementRole::None)
			{
				AvailableEnemies.Add(Enemy);
			}
		}

		if (AvailableEnemies.Num() == 0)
		{
			break;
		}

		AEnemyBaseCharacter* RandomEnemy = AvailableEnemies[FMath::RandRange(0, AvailableEnemies.Num() - 1)];
		UGroupCombatComponent* GCC = RandomEnemy->FindComponentByClass<UGroupCombatComponent>();

		GCC->EngagementRole = InRole;
		ResetTimeSpentInRole(RandomEnemy);
	}
}

void UGroupCombatSubsystem::AssignAllWeightedRandomSelectionEngagementRole(EEnemyEngagementRole InRole, TArray<AEnemyBaseCharacter*> InEnemies)
{
	
	float RoleWeightTotal = 0;
	
	if(InRole == EEnemyEngagementRole::Aggressor)
	{
		for(auto EnemyChar : InEnemies)
		{
			RoleWeightTotal += EnemyChar->AICombatParams->AggressionLevel;
		}
	}
	else if(InRole == EEnemyEngagementRole::Cover)
	{
		for(auto EnemyChar : InEnemies)
		{
			RoleWeightTotal += EnemyChar->AICombatParams->CoverLevel;
		}
	}
	else if(InRole == EEnemyEngagementRole::Observer)
	{
		for(auto EnemyChar : InEnemies)
		{
			RoleWeightTotal += EnemyChar->AICombatParams->ObserverLevel;
		}
	}

	float RandomVal = FMath::FRandRange(0, RoleWeightTotal);

	for (auto EnemyChar : InEnemies)
	{
		float RoleLevel = 0.0f;
		
		if(InRole == EEnemyEngagementRole::Aggressor)
		{
			RoleLevel = EnemyChar->AICombatParams->AggressionLevel;
		}
		else if(InRole == EEnemyEngagementRole::Cover)
		{
			RoleLevel = EnemyChar->AICombatParams->CoverLevel;
		}
		else if(InRole == EEnemyEngagementRole::Observer)
		{
			RoleLevel = EnemyChar->AICombatParams->ObserverLevel;
		}
		
		if(RoleLevel == 0.0f)
		{
			continue;
		}
		else
		{
			RandomVal -= RoleWeightTotal;
	
			if(RandomVal <= 0.0f)
			{
				UGroupCombatComponent* GCC = EnemyChar->FindComponentByClass<UGroupCombatComponent>();
				GCC->EngagementRole = InRole;
				ResetTimeSpentInRole(EnemyChar);
				break;
			}
		}
		return;
	}
}

TArray<AEnemyBaseCharacter*> UGroupCombatSubsystem::GetAllAvailableEnemies()
{
	TArray<AEnemyBaseCharacter*> AvailableEnemies;

	for (AEnemyBaseCharacter* Enemy : AllEnemiesArray)
	{
		if (!Enemy)
		{
			continue;
		}

		UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();

		if (GCC && GCC->EngagementRole == EEnemyEngagementRole::None)
		{
			AvailableEnemies.Add(Enemy);
		}
	}

	if (AvailableEnemies.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No Available Enemies"));
		return AvailableEnemies;
	}
	return AvailableEnemies;
	// Algo::RandomShuffle(AvailableEnemies);
}

void UGroupCombatSubsystem::AssignEngagementRole(TObjectPtr<AEnemyBaseCharacter> EnemyChar, EEnemyEngagementRole InRole)
{
	
	UGroupCombatComponent* GCC = EnemyChar->FindComponentByClass<UGroupCombatComponent>();
	
	// UE_LOG(LogTemp, Warning, TEXT("AssignEngagementRole - Time spent in current role: %f"), GetWorld()->GetTimerManager().GetTimerElapsed(GCC->CurrentRoleTimer));
	float TimeSpentInRole = GetWorld()->GetTimeSeconds() - GCC->RoleTimerStartTime;


	ResetTimeSpentInRole(EnemyChar);
	GCC->EngagementRole = InRole;
	EnforceAllEngagementRoleCounts();
	UE_LOG(LogTemp, Warning, TEXT("After - Aggressors: %d"), GetNumOfRoles(EEnemyEngagementRole::Aggressor));

	if(InRole == EEnemyEngagementRole::Aggressor)
	{
		if (AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(EnemyChar->GetController()))
		{
			const UFTAStateTreeAIComponent* STComp = EnemyController->StateTreeComponent;

			if (STComp)
			{
				FStateTreeEvent AttackEvent;
				AttackEvent.Tag = FGameplayTag::RequestGameplayTag("StateTreeTag.State.Attacking");

				EnemyController->StateTreeComponent->SendStateTreeEvent(AttackEvent);
			}
		}
	}

	else if(InRole == EEnemyEngagementRole::Cover)
	{
		if (AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(EnemyChar->GetController()))
		{
			const UFTAStateTreeAIComponent* STComp = EnemyController->StateTreeComponent;

			if (STComp)
			{
				FStateTreeEvent AttackEvent;
				AttackEvent.Tag = FGameplayTag::RequestGameplayTag("StateTreeTag.State.Active");

				EnemyController->StateTreeComponent->SendStateTreeEvent(AttackEvent);
			}
		}
	}
	else if(InRole == EEnemyEngagementRole::Observer)
	{
		if (AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(EnemyChar->GetController()))
		{
			const UFTAStateTreeAIComponent* STComp = EnemyController->StateTreeComponent;

			if (STComp)
			{
				FStateTreeEvent AttackEvent;
				AttackEvent.Tag = FGameplayTag::RequestGameplayTag("StateTreeTag.State.Active");

				EnemyController->StateTreeComponent->SendStateTreeEvent(AttackEvent);
			}
		}
	}

	EnforceAllEngagementRoleCounts();
}

void UGroupCombatSubsystem::EnforceEngagementRoleCount(EEnemyEngagementRole InRole)
{
	AFTAGameModeBase* FTAGameMode = Cast<AFTAGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if(!FTAGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid gamemode"));
		return;
	}

	UEnemyEncounterDataAsset* EncounterData = FTAGameMode->EnemyEncounterArray[FTAGameMode->CurrentEncounter];
	
	int32 MinCount;
	int32 CurrentCount = 0;
	int32 MaxCount;
	
	if(InRole == EEnemyEngagementRole::Aggressor)
	{
		MinCount = EncounterData->AggressorRoles.MinimumRoleCount;
		MaxCount = EncounterData->AggressorRoles.MaximumRoleCount;
	}
	else if(InRole == EEnemyEngagementRole::Cover)
	{
		MinCount = EncounterData->CoverRoles.MinimumRoleCount;
		MaxCount = EncounterData->CoverRoles.MaximumRoleCount;
	}
	else if(InRole == EEnemyEngagementRole::Observer)
	{
		MinCount = EncounterData->ObserverRoles.MinimumRoleCount;
		MaxCount = EncounterData->ObserverRoles.MaximumRoleCount;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGroupCombatSubsystem::AssignEngagementRole - Invalid Role"))
		return;
	}

	TArray<AEnemyBaseCharacter*> MatchingEnemies;
	TArray<AEnemyBaseCharacter*> OtherEnemies;

	for(auto Enemy : AllEnemiesArray)
	{
		if(!Enemy)
		{
			continue;
		}

		if(UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>())
		{
			if(GCC->EngagementRole == InRole)
			{
				MatchingEnemies.Add(Enemy);
				CurrentCount++;
			}
			else
			{
				OtherEnemies.Add(Enemy);
			}
		}
	}

	if(CurrentCount < MinCount)
	{
		int32 RoleAdditionsNeeded = MinCount - CurrentCount;

		for(int32 i = 0; i < RoleAdditionsNeeded; i++)
		{
			AEnemyBaseCharacter* RandomEnemy = OtherEnemies[FMath::RandRange(0, OtherEnemies.Num() - 1)];
			
			UGroupCombatComponent* GCC = RandomEnemy->FindComponentByClass<UGroupCombatComponent>();
			
			GCC->EngagementRole = InRole;
			
			OtherEnemies.Remove(RandomEnemy);
		}
	}
	else if(CurrentCount > MaxCount)
	{
		int32 RoleRemovalsNeeded = CurrentCount - MaxCount;
		for(int32 i = 0; i < RoleRemovalsNeeded; i++)
		{
			AEnemyBaseCharacter* RandomEnemy = MatchingEnemies[FMath::RandRange(0, MatchingEnemies.Num() - 1)];
			
			UGroupCombatComponent* GCC = RandomEnemy->FindComponentByClass<UGroupCombatComponent>();
			
			GCC->EngagementRole = EEnemyEngagementRole::Observer;
			UE_LOG(LogTemp, Warning, TEXT("Im an observer"))
			MatchingEnemies.Remove(RandomEnemy);
		}
	}
	else if(CurrentCount == MaxCount)
	{
		return;
	}
	else
	{
		return;
	}
	
}

void UGroupCombatSubsystem::EnforceAllEngagementRoleCounts()
{
	EnforceEngagementRoleCount(EEnemyEngagementRole::Cover);
	EnforceEngagementRoleCount(EEnemyEngagementRole::Observer);
	EnforceEngagementRoleCount(EEnemyEngagementRole::Aggressor);
}

void UGroupCombatSubsystem::SwapOutAggressor(TObjectPtr<AEnemyBaseCharacter> InEnemy)
{
	if (!InEnemy)
	{
		return;
	}

	if(AllEnemiesArray.Num() < 2)
	{
		return;
	}

	UGroupCombatComponent* InGCC =
		InEnemy->FindComponentByClass<UGroupCombatComponent>();

	// UE_LOG(LogTemp, Warning, TEXT("Swap out aggressor - Time spent in current role: %f"), GetWorld()->GetTimerManager().GetTimerElapsed(InGCC->CurrentRoleTimer));

	float TimeSpentInRole = GetWorld()->GetTimeSeconds() - InGCC->RoleTimerStartTime;
	UE_LOG(LogTemp, Warning, TEXT("Swap out aggressor - Time spent in current role: %f"), TimeSpentInRole);
	ResetTimeSpentInRole(InEnemy);

	if (InGCC)
	{
		InGCC->EngagementRole = EEnemyEngagementRole::Cover;
	}

	TArray<AEnemyBaseCharacter*> OtherEnemies;

	for (AEnemyBaseCharacter* CurrentEnemy : AllEnemiesArray)
	{
		if (!CurrentEnemy || CurrentEnemy == InEnemy)
		{
			continue;
		}

		OtherEnemies.Add(CurrentEnemy);
	}

	AEnemyBaseCharacter* RandomEnemy = OtherEnemies[FMath::RandRange(0, OtherEnemies.Num() - 1)];
	UGroupCombatComponent* GCC = RandomEnemy->FindComponentByClass<UGroupCombatComponent>();

	float TimeSpentInRole_2 = GetWorld()->GetTimeSeconds() - GCC->RoleTimerStartTime;
	UE_LOG(LogTemp, Warning, TEXT("Swap out aggressor - Time spent in current role_2: %f"), TimeSpentInRole_2);
	ResetTimeSpentInRole(RandomEnemy);

	GCC->EngagementRole = EEnemyEngagementRole::Aggressor;
	
	if (AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(RandomEnemy->GetController()))
	{
		const UFTAStateTreeAIComponent* STComp = EnemyController->StateTreeComponent;

		if (STComp)
		{
			FStateTreeEvent AttackEvent;
			AttackEvent.Tag = FGameplayTag::RequestGameplayTag("StateTreeTag.State.Attacking");

			EnemyController->StateTreeComponent->SendStateTreeEvent(AttackEvent);
		}
	}

	EnforceAllEngagementRoleCounts();

	/*UE_LOG(LogTemp, Warning, TEXT("Aggressor Count: %d"), PrintNumOfRoles(EEnemyEngagementRole::Aggressor))
	UE_LOG(LogTemp, Warning, TEXT("Cover Count: %d"), PrintNumOfRoles(EEnemyEngagementRole::Cover))
	UE_LOG(LogTemp, Warning, TEXT("Observer Count: %d"), PrintNumOfRoles(EEnemyEngagementRole::Observer))
	UE_LOG(LogTemp, Warning, TEXT("None Count: %d"), PrintNumOfRoles(EEnemyEngagementRole::None))*/

}


void UGroupCombatSubsystem::ActivateAllStateTrees()
{
	for(auto Enemy : AllEnemiesArray)
	{
		if(Enemy)
		{
			if(Enemy->GetController())
			{
				AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(Enemy->GetController());
				EnemyController->StateTreeComponent->StartLogic();
			}
		}
	}
}

int32 UGroupCombatSubsystem::GetNumOfRoles(EEnemyEngagementRole InRole)
{
	int32 Count = 0;
	for (AEnemyBaseCharacter* Enemy : AllEnemiesArray)
	{
		UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();
		{
			if(GCC)
			{
				if(GCC->EngagementRole == InRole)
				{
					Count++;
				}
			}
		}
	}
	return Count;
}

void UGroupCombatSubsystem::ResetTimeSpentInRole(TObjectPtr<AEnemyBaseCharacter> Enemy)
{
	UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();
	GCC->StartRoleTimer();
}

void UGroupCombatSubsystem::FlipAggression()
{
	UE_LOG(LogTemp, Warning, TEXT("Flip"))
	
	AFTAGameModeBase* FTAGameMode = Cast<AFTAGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if(!FTAGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid gamemode"));
		return;
	}

	UEnemyEncounterDataAsset* EncounterData = FTAGameMode->EnemyEncounterArray[FTAGameMode->CurrentEncounter];

	if(EncounterData)
	{
		if(Flip)
		{
			Flip = false;
			EncounterData->AggressorRoles.MaximumRoleCount = 2;
			EncounterData->AggressorRoles.MinimumRoleCount = 2;
		}
		else
		{
			Flip = true;
			EncounterData->AggressorRoles.MaximumRoleCount = 1;
			EncounterData->AggressorRoles.MinimumRoleCount = 1;
		}
	}

	EnforceAllEngagementRoleCounts();
}
