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

	AssignInitialRoles(EEnemyEngagementRole::Aggressor, EncounterData->AggressorRoles.StartingRoleCount);
	AssignInitialRoles(EEnemyEngagementRole::Cover, EncounterData->CoverRoles.StartingRoleCount);
	AssignInitialRoles(EEnemyEngagementRole::Observer, EncounterData->ObserverRoles.StartingRoleCount);
	

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

void UGroupCombatSubsystem::AssignInitialRoles(EEnemyEngagementRole Role, int32 StartingRoleCount)
{
	TArray<AEnemyBaseCharacter*> GuaranteedEnemies;
	
	for(auto Enemy : GetAllAvailableEnemies())
	{
		if(!Enemy || !Enemy->AICombatParams)
		{
			continue;
		}
		float InitialWeight = 0.0f;
		
		switch (Role)
		{
		case EEnemyEngagementRole::Aggressor:
			InitialWeight = Enemy->AICombatParams->AggressionStats.InitialWeight;
			break;

		case EEnemyEngagementRole::Cover:
			InitialWeight = Enemy->AICombatParams->CoverStats.InitialWeight;
			break;

		case EEnemyEngagementRole::Observer:
			InitialWeight = Enemy->AICombatParams->ObserverStats.InitialWeight;
			break;

		default:
			break;
		}

		if (InitialWeight == 1.0f)
		{
			GuaranteedEnemies.Add(Enemy);
		}
	}
	
	for (int32 i = 0; i < StartingRoleCount; i++)
	{
		if (GuaranteedEnemies.Num() > 0)
		{
			const int32 RandomIndex = FMath::RandRange(0, GuaranteedEnemies.Num() - 1);
			AEnemyBaseCharacter* Enemy = GuaranteedEnemies[RandomIndex];

			if (!Enemy)
			{
				continue;
			}
			
			AssignEngagementRole(Enemy, Role);
			GuaranteedEnemies.RemoveAt(RandomIndex);
		}
		else
		{
			AssignInitialWeightedRandomEngagementRole(Role, GetAllAvailableEnemies());
		}
	}
}

void UGroupCombatSubsystem::AssignInitialWeightedRandomEngagementRole(EEnemyEngagementRole InRole, TArray<AEnemyBaseCharacter*> InEnemies)
{
	float RoleWeightTotal = 0;
	
	if(InRole == EEnemyEngagementRole::Aggressor)
	{
		for(auto EnemyChar : InEnemies)
		{
			RoleWeightTotal += EnemyChar->AICombatParams->AggressionStats.InitialWeight;
		}
	}
	else if(InRole == EEnemyEngagementRole::Cover)
	{
		for(auto EnemyChar : InEnemies)
		{
			RoleWeightTotal += EnemyChar->AICombatParams->CoverStats.InitialWeight;
		}
	}
	else if(InRole == EEnemyEngagementRole::Observer)
	{
		for(auto EnemyChar : InEnemies)
		{
			RoleWeightTotal += EnemyChar->AICombatParams->ObserverStats.InitialWeight;
		}
	}

	Algo::RandomShuffle(InEnemies);
	

	float RandomVal = FMath::FRandRange(0, RoleWeightTotal);

	for (auto EnemyChar : InEnemies)
	{
		float RoleLevel = 0.0f;
		
		if(InRole == EEnemyEngagementRole::Aggressor)
		{
			RoleLevel = EnemyChar->AICombatParams->AggressionStats.InitialWeight;
		}
		else if(InRole == EEnemyEngagementRole::Cover)
		{
			RoleLevel = EnemyChar->AICombatParams->CoverStats.InitialWeight;
		}
		else if(InRole == EEnemyEngagementRole::Observer)
		{
			RoleLevel = EnemyChar->AICombatParams->ObserverStats.InitialWeight;
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
				AssignEngagementRole(EnemyChar, InRole);
				break;
			}
		}
		return;
	}
}

void UGroupCombatSubsystem::AssignWeightedRandomEngagementRole(EEnemyEngagementRole InRole, TArray<AEnemyBaseCharacter*> InEnemies)
{
	float RoleWeightTotal = 0;
	
	if(InRole == EEnemyEngagementRole::Aggressor)
	{
		for(auto EnemyChar : InEnemies)
		{
			RoleWeightTotal += EnemyChar->AICombatParams->AggressionStats.BaseWeight;
		}
	}
	else if(InRole == EEnemyEngagementRole::Cover)
	{
		for(auto EnemyChar : InEnemies)
		{
			RoleWeightTotal += EnemyChar->AICombatParams->CoverStats.BaseWeight;
		}
	}
	else if(InRole == EEnemyEngagementRole::Observer)
	{
		for(auto EnemyChar : InEnemies)
		{
			RoleWeightTotal += EnemyChar->AICombatParams->ObserverStats.BaseWeight;
		}
	}

	Algo::RandomShuffle(InEnemies);
	

	float RandomVal = FMath::FRandRange(0, RoleWeightTotal);

	for (auto EnemyChar : InEnemies)
	{
		float RoleLevel = 0.0f;
		
		if(InRole == EEnemyEngagementRole::Aggressor)
		{
			RoleLevel = EnemyChar->AICombatParams->AggressionStats.BaseWeight;
		}
		else if(InRole == EEnemyEngagementRole::Cover)
		{
			RoleLevel = EnemyChar->AICombatParams->CoverStats.BaseWeight;
		}
		else if(InRole == EEnemyEngagementRole::Observer)
		{
			RoleLevel = EnemyChar->AICombatParams->ObserverStats.BaseWeight;
		}
		
		if(RoleLevel == 0.0f)
		{
			continue;
		}
		
		RandomVal -= RoleWeightTotal;

		if(RandomVal <= 0.0f)
		{
			
			AssignEngagementRole(EnemyChar, InRole);
			break;
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
}

void UGroupCombatSubsystem::AssignEngagementRole(TObjectPtr<AEnemyBaseCharacter> EnemyChar, EEnemyEngagementRole InRole)
{
	UGroupCombatComponent* GCC = EnemyChar->FindComponentByClass<UGroupCombatComponent>();
	
	float TimeSpentInRole = GetWorld()->GetTimeSeconds() - GCC->RoleTimerStartTime;
	
	ResetTimeSpentInRole(EnemyChar);
	GCC->EngagementRole = InRole;

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
			AssignEngagementRole(RandomEnemy, InRole);
			OtherEnemies.Remove(RandomEnemy);
		}
	}
	else if(CurrentCount > MaxCount)
	{
		int32 RoleRemovalsNeeded = CurrentCount - MaxCount;
		for(int32 i = 0; i < RoleRemovalsNeeded; i++)
		{
			AEnemyBaseCharacter* RandomEnemy = MatchingEnemies[FMath::RandRange(0, MatchingEnemies.Num() - 1)];
			AssignEngagementRole(RandomEnemy, EEnemyEngagementRole::Observer);
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
	for(auto Enemy : AllEnemiesArray)
	{
		if(Enemy)
		{
			float DistToTarget = FVector::Dist(Enemy->GetActorLocation(), UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation());
			float DistRatio = DistToTarget / 200;
			
			UE_LOG(LogTemp, Warning, TEXT("Enemy: %s - Ratio: %f"), *GetNameSafe(Enemy), DistRatio);
		}
	}
	
	if (!InEnemy)
	{
		return;
	}

	if(AllEnemiesArray.Num() < 2)
	{
		return;
	}
	
	AssignEngagementRole(InEnemy, EEnemyEngagementRole::Cover);

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
	AssignEngagementRole(RandomEnemy, EEnemyEngagementRole::Aggressor);
	
	
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
