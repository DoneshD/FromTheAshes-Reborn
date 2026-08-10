#include "Enemy/GroupCombatSubsystem.h"

#include "AbilitySystemComponent.h"
#include "StateTreeEvents.h"
#include "CombatComponents/GroupCombatComponent.h"
#include "DataAsset/EnemyEncounterDataAsset.h"
#include "Enemy/AIControllerEnemyBase.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "Enemy/FTAStateTreeAIComponent.h"
#include "GameModes/FTAGameModeBase.h"
#include "Kismet/GameplayStatics.h"
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

/*void UGroupCombatSubsystem::RegisterEnemyToGroupCombat()
{
	AllEnemiesArray.Add(Enemy);
	
	UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();
	
	if(GetAggressorCount() < 1)
	{
		GCC->EngagementRole = EEnemyEngagementRole::Aggressor;
	}
	else
	{
		GCC->EngagementRole = EEnemyEngagementRole::Cover;
	}
	
	UAbilitySystemComponent* ASC = Enemy->FindComponentByClass<UAbilitySystemComponent>();
	UE_LOG(LogTemp, Warning, TEXT("Enemy listening: %s"), *Enemy->GetName());

	ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("HitTag.State.Hit"), EGameplayTagEventType::NewOrRemoved).AddLambda(
			[this, Enemy](const FGameplayTag Tag, int32 NewCount)
			{
				if (NewCount > 0)
				{
					SwapOutAggressor(Enemy);
				}
			}
		);
	
}*/

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

	AssignEngagementRole(EncounterData, EEnemyEngagementRole::Aggressor);
	AssignEngagementRole(EncounterData, EEnemyEngagementRole::Cover);
	AssignEngagementRole(EncounterData, EEnemyEngagementRole::Observer);
	
	for (AEnemyBaseCharacter* Enemy : AllEnemiesArray)
	{
		if (Enemy)
		{
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
}

void UGroupCombatSubsystem::AssignEngagementRole(UEnemyEncounterDataAsset* InEncounterData, EEnemyEngagementRole InRole)
{
	int32 RoleCount;

	if(InRole == EEnemyEngagementRole::Aggressor)
	{
		RoleCount = InEncounterData->AggressorRoles.CurrentRoleCount;
	}
	else if(InRole == EEnemyEngagementRole::Cover)
	{
		RoleCount = InEncounterData->CoverRoles.CurrentRoleCount;
	}
	else if(InRole == EEnemyEngagementRole::Observer)
	{
		RoleCount = InEncounterData->ObserverRoles.CurrentRoleCount;
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
	}
}

void UGroupCombatSubsystem::SwapOutAggressor(TObjectPtr<AEnemyBaseCharacter> InEnemy)
{
	if (!InEnemy)
	{
		return;
	}

	UGroupCombatComponent* InGCC =
		InEnemy->FindComponentByClass<UGroupCombatComponent>();

	if (InGCC)
	{
		InGCC->EngagementRole = EEnemyEngagementRole::Cover;
	}

	TArray<AEnemyBaseCharacter*> CoverEnemies;

	for (AEnemyBaseCharacter* CurrentEnemy : AllEnemiesArray)
	{
		if (!CurrentEnemy || CurrentEnemy == InEnemy)
		{
			continue;
		}

		UGroupCombatComponent* CurrentGCC =
			CurrentEnemy->FindComponentByClass<UGroupCombatComponent>();

		if (!CurrentGCC)
		{
			continue;
		}

		if (CurrentGCC->EngagementRole == EEnemyEngagementRole::Cover)
		{
			CoverEnemies.Add(CurrentEnemy);
		}
	}

	if (CoverEnemies.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, CoverEnemies.Num() - 1);

		AEnemyBaseCharacter* NewAggressor = CoverEnemies[RandomIndex];

		UGroupCombatComponent* NewAggressorGCC = NewAggressor->FindComponentByClass<UGroupCombatComponent>();

		if (NewAggressorGCC)
		{
			NewAggressorGCC->EngagementRole = EEnemyEngagementRole::Aggressor;

			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Swapped aggressor: %s -> %s"),
				*InEnemy->GetName(),
				*NewAggressor->GetName()
			);

			if (AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(NewAggressor->GetController()))
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

		for (AEnemyBaseCharacter* Enemy : AllEnemiesArray)
		{
			if(Enemy)
			{
				if(Enemy != NewAggressor)
				{
					UGroupCombatComponent* GCCEnemy = Enemy->FindComponentByClass<UGroupCombatComponent>();
					if(GCCEnemy)
					{
						GCCEnemy->EngagementRole = EEnemyEngagementRole::Cover;
					}
				}
			}
		}
	}
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