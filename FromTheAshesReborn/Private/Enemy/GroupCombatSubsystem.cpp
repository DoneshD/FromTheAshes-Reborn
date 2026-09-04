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

	Algo::RandomShuffle(AllEnemiesArray);
	
	UEnemyEncounterDataAsset* EncounterData = FTAGameMode->EnemyEncounterArray[FTAGameMode->CurrentEncounter];
	
	AllRoleRequirements.Add(EncounterData->AggressorRoles);
	AllRoleRequirements.Add(EncounterData->CoverRoles);
	AllRoleRequirements.Add(EncounterData->ObserverRoles);

	AssignInitialRoles(EEnemyEngagementRole::Aggressor, EncounterData->AggressorRoles.StartingCount);
	AssignInitialRoles(EEnemyEngagementRole::Cover, EncounterData->CoverRoles.StartingCount);
	AssignInitialRoles(EEnemyEngagementRole::Observer, EncounterData->ObserverRoles.StartingCount);
	

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
	
	EEnemyEngagementRole AggressorRole = EEnemyEngagementRole::Aggressor;

	GetWorld()->GetTimerManager().SetTimer(
		AddTimer,
		[this, AggressorRole]()
		{
			AddRole(AggressorRole);
		},
		2.0f,
		false
	);
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
			RandomVal -= RoleLevel;
	
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
		
		RandomVal -= RoleLevel;

		if(RandomVal <= 0.0f)
		{
			AssignEngagementRole(EnemyChar, InRole);
			break;
		}
	}
}

AEnemyBaseCharacter* UGroupCombatSubsystem::SelectWeightedRandomEnemy(EEnemyEngagementRole InRole, TArray<AEnemyBaseCharacter*>& InEnemies)
{
	
	float RoleWeightTotal = 0;
	
	if(InRole == EEnemyEngagementRole::Aggressor)
	{
		for(auto EnemyChar : InEnemies)
		{
			RoleWeightTotal += EnemyChar->AICombatParams->AggressionStats.FinalWeight;
			// UE_LOG(LogTemp, Warning, TEXT("Enemy: %s - Final Weight: %f"), *GetNameSafe(EnemyChar), EnemyChar->AICombatParams->AggressionStats.FinalWeight)
		}
	}
	else if(InRole == EEnemyEngagementRole::Cover)
	{
		for(auto EnemyChar : InEnemies)
		{
			RoleWeightTotal += EnemyChar->AICombatParams->CoverStats.FinalWeight;
		}
	}
	else if(InRole == EEnemyEngagementRole::Observer)
	{
		for(auto EnemyChar : InEnemies)
		{
			RoleWeightTotal += EnemyChar->AICombatParams->ObserverStats.FinalWeight;
		}
	}
	
	
	float RandomVal = FMath::FRandRange(0, RoleWeightTotal);

	for (auto EnemyChar : InEnemies)
	{
		float RoleLevel = 0.0f;
		
		if(InRole == EEnemyEngagementRole::Aggressor)
		{
			RoleLevel = EnemyChar->AICombatParams->AggressionStats.FinalWeight;
			
		}
		else if(InRole == EEnemyEngagementRole::Cover)
		{
			RoleLevel = EnemyChar->AICombatParams->CoverStats.FinalWeight;
		}
		else if(InRole == EEnemyEngagementRole::Observer)
		{
			RoleLevel = EnemyChar->AICombatParams->ObserverStats.FinalWeight;
		}
		
		if(RoleLevel == 0.0f)
		{
			continue;
		}
		
		RandomVal -= RoleLevel;

		if(RandomVal <= 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Selected for addtion enemy: %s"), *GetNameSafe(EnemyChar));
			return EnemyChar;
		}
	}
	return nullptr;
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
			GetWorld()->GetTimerManager().SetTimer(
				GCC->AttackTimer,
				[this, EnemyController]()
				{
					StartAttacking(EnemyController);
				},
				1.0f,
				false
			);
			// FStateTreeEvent AttackEvent;
			// AttackEvent.Tag = FGameplayTag::RequestGameplayTag("StateTreeTag.State.Attacking");
			//
			// EnemyController->StateTreeComponent->SendStateTreeEvent(AttackEvent);
		}
	}

	else if(InRole == EEnemyEngagementRole::Cover)
	{
		GetWorld()->GetTimerManager().ClearTimer(GCC->AttackTimer);
		if (AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(EnemyChar->GetController()))
		{
			const UFTAStateTreeAIComponent* STComp = EnemyController->StateTreeComponent;

			if (STComp)
			{
				FStateTreeEvent CoverEvent;
				CoverEvent.Tag = FGameplayTag::RequestGameplayTag("StateTreeTag.State.Active");

				EnemyController->StateTreeComponent->SendStateTreeEvent(CoverEvent);
			}
		}
	}
	else if(InRole == EEnemyEngagementRole::Observer)
	{
		GetWorld()->GetTimerManager().ClearTimer(GCC->AttackTimer);
		if (AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(EnemyChar->GetController()))
		{
			const UFTAStateTreeAIComponent* STComp = EnemyController->StateTreeComponent;

			if (STComp)
			{
				FStateTreeEvent ObserveEvent;
				ObserveEvent.Tag = FGameplayTag::RequestGameplayTag("StateTreeTag.State.Active");

				EnemyController->StateTreeComponent->SendStateTreeEvent(ObserveEvent);
			}
		}
	}

}

void UGroupCombatSubsystem::StartAttacking(TObjectPtr<AAIControllerEnemyBase> InEnemyController)
{
	FStateTreeEvent AttackEvent;
	AttackEvent.Tag = FGameplayTag::RequestGameplayTag("StateTreeTag.State.Attacking");
	
	InEnemyController->StateTreeComponent->SendStateTreeEvent(AttackEvent);
}

void UGroupCombatSubsystem::EnforceAllEngagementRoleCounts()
{
	TArray<FRoleRequirement> Requirements;
	TArray<AEnemyBaseCharacter*> ValidEnemies;
	
	GetRoleCountsFromValidEnemies(Requirements, ValidEnemies);
	
	bool MadeChange = true;

	while(MadeChange)
	{
		SatisfyMinimumRoleCounts(Requirements, ValidEnemies);
		MadeChange = false;
	}

	SatisfyMaximumRoleCounts(Requirements, ValidEnemies);
	
	/*for(const FRoleRequirement& Requirement : Requirements)
	{
		UE_LOG(LogTemp, Log, TEXT("Role %s: %d / Min %d / Max %d"),
			*UEnum::GetValueAsString(Requirement.Role),
			Requirement.CurrentCount,
			Requirement.MinCount,
			Requirement.MaxCount);
	}*/
}

void UGroupCombatSubsystem::GetRoleCountsFromValidEnemies(TArray<FRoleRequirement>& OutRoleRequirements, TArray<AEnemyBaseCharacter*>& OutValidEnemies)
{
	AFTAGameModeBase* FTAGameMode = Cast<AFTAGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if(!FTAGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid gamemode"));
		return;
	}

	if(!FTAGameMode->EnemyEncounterArray.IsValidIndex(FTAGameMode->CurrentEncounter))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid current encounter index"));
		return;
	}

	UEnemyEncounterDataAsset* EncounterData = FTAGameMode->EnemyEncounterArray[FTAGameMode->CurrentEncounter];

	if(!EncounterData)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid EncounterData"));
		return;
	}

	TArray<FRoleRequirement> RoleRequirements =
	{
		{
			EEnemyEngagementRole::Aggressor,
			EncounterData->AggressorRoles.MinCount,
			EncounterData->AggressorRoles.MaxCount
		},

		{
			EEnemyEngagementRole::Cover,
			EncounterData->CoverRoles.MinCount,
			EncounterData->CoverRoles.MaxCount
		},

		{
			EEnemyEngagementRole::Observer,
			EncounterData->ObserverRoles.MinCount,
			EncounterData->ObserverRoles.MaxCount
		}
	};
	
	Algo::RandomShuffle(RoleRequirements);

	TArray<AEnemyBaseCharacter*> ValidEnemies;

	for(AEnemyBaseCharacter* Enemy : AllEnemiesArray)
	{
		if(Enemy)
		{
			ValidEnemies.Add(Enemy);
		}
	}

	for(AEnemyBaseCharacter* Enemy : ValidEnemies)
	{
		UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();

		if(!GCC)
		{
			continue;
		}

		for(FRoleRequirement& Requirement : RoleRequirements)
		{
			if(GCC->EngagementRole == Requirement.Role)
			{
				Requirement.CurrentCount++;
				break;
			}
		}
	}

	int32 TotalMinimum = 0;
	int32 TotalMaximum = 0;

	for(const FRoleRequirement& Requirement : RoleRequirements)
	{
		TotalMinimum += Requirement.MinCount;
		TotalMaximum += Requirement.MaxCount;
	}

	if(ValidEnemies.Num() < TotalMinimum)
	{
		UE_LOG(LogTemp, Error, TEXT("Too many enemies - Enemies: %d, Minimum Required: %d"), ValidEnemies.Num(), TotalMinimum);
	}

	if(ValidEnemies.Num() > TotalMaximum)
	{
		UE_LOG(LogTemp, Warning, TEXT("Too few enemies - Enemies: %d, Maximum Allowed: %d"), ValidEnemies.Num(), TotalMaximum);
	}
	
	OutRoleRequirements = RoleRequirements;
	OutValidEnemies = ValidEnemies;
}

void UGroupCombatSubsystem::SatisfyMinimumRoleCounts(TArray<FRoleRequirement>& InRequirements, TArray<AEnemyBaseCharacter*>& InValidEnemies)
{
	for(FRoleRequirement& TargetRequirement : InRequirements)
	{
		if(TargetRequirement.CurrentCount >= TargetRequirement.MinCount)
		{
			continue;
		}

		TArray<AEnemyBaseCharacter*> Candidates;
			
		Algo::RandomShuffle(InValidEnemies);

		for(AEnemyBaseCharacter* Enemy : InValidEnemies)
		{
			UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();

			if(!GCC)
			{
				continue;
			}

			if(GCC->EngagementRole == TargetRequirement.Role)
			{
				continue;
			}

			FRoleRequirement* CurrentRequirement = nullptr;

			for(FRoleRequirement& Requirement : InRequirements)
			{
				if(Requirement.Role == GCC->EngagementRole)
				{
					CurrentRequirement = &Requirement;
					break;
				}
			}

			if(!CurrentRequirement)
			{
				continue;
			}

			/*if(CurrentRequirement->CurrentCount > CurrentRequirement->MinCount)
			{
				Candidates.Add(Enemy);
			}*/

			Candidates.Add(Enemy);
				
		}

		if(Candidates.Num() == 0)
		{
			continue;
		}
			
		AEnemyBaseCharacter* SelectedEnemy = SelectWeightedRandomEnemy(TargetRequirement.Role, Candidates);

		if(!SelectedEnemy)
		{
			continue;
		}

		UGroupCombatComponent* GCC = SelectedEnemy->FindComponentByClass<UGroupCombatComponent>();

		if(!GCC)
		{
			continue;
		}

		const EEnemyEngagementRole OldRole = GCC->EngagementRole;

		for(FRoleRequirement& Requirement : InRequirements)
		{
			if(Requirement.Role == OldRole)
			{
				Requirement.CurrentCount--;
				break;
			}
		}

		AssignEngagementRole(SelectedEnemy, TargetRequirement.Role);
		TargetRequirement.CurrentCount++;
	}
}

void UGroupCombatSubsystem::SatisfyMaximumRoleCounts(TArray<FRoleRequirement>& InRequirements, TArray<AEnemyBaseCharacter*>& InValidEnemies)
{
	for(FRoleRequirement& Requirement : InRequirements)
	{
		while(Requirement.CurrentCount > Requirement.MaxCount)
		{
			AEnemyBaseCharacter* SelectedEnemy = nullptr;

			for(AEnemyBaseCharacter* Enemy : InValidEnemies)
			{
				UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();

				if(!GCC)
				{
					continue;
				}

				if(GCC->EngagementRole == Requirement.Role)
				{
					SelectedEnemy = Enemy;
					break;
				}
			}

			if(!SelectedEnemy)
			{
				break;
			}

			FRoleRequirement* DestinationRequirement = nullptr;

			for(FRoleRequirement& OtherRequirement : InRequirements)
			{
				if(OtherRequirement.Role == Requirement.Role)
				{
					continue;
				}

				if(OtherRequirement.CurrentCount < OtherRequirement.MaxCount)
				{
					DestinationRequirement = &OtherRequirement;
					break;
				}
			}

			if(!DestinationRequirement)
			{
				UE_LOG(LogTemp, Warning, TEXT("No available role to move excess enemy from role %s"), *UEnum::GetValueAsString(Requirement.Role));
				break;
			}

			AssignEngagementRole(SelectedEnemy, DestinationRequirement->Role);

			Requirement.CurrentCount--;
			DestinationRequirement->CurrentCount++;
		}
	}
}


void UGroupCombatSubsystem::SwapOutAggressor(TObjectPtr<AEnemyBaseCharacter> InEnemy)
{
	for(AEnemyBaseCharacter* Enemy : AllEnemiesArray)
	{
		if(Enemy)
		{
			float DistToTarget = FVector::Dist(Enemy->GetActorLocation(), UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation());
			float DistRatio = DistToTarget / 200;
			
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
	
	int32 Choice = FMath::RandRange(0, 1);

	if (Choice == 0)
	{
		AssignEngagementRole(InEnemy, EEnemyEngagementRole::Cover);
	}
	else
	{
		AssignEngagementRole(InEnemy, EEnemyEngagementRole::Observer);
	}
	
	EnforceAllEngagementRoleCounts();
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

int32 UGroupCombatSubsystem::GetNumInRoles(EEnemyEngagementRole InRole)
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

void UGroupCombatSubsystem::ChangeMinMaxRoleCount(EEnemyEngagementRole InRole, int32 InMinCount, int32 InMaxCount)
{
	for(FRoleRequirement& RoleRequirement : AllRoleRequirements)
	{
		if(InRole == RoleRequirement.Role)
		{
			RoleRequirement.MaxCount = InMaxCount;
			RoleRequirement.MinCount = InMinCount;
			break;
		}
	}
}

void UGroupCombatSubsystem::AddRole(EEnemyEngagementRole InRole)
{
	int32 MaxCount = 0;

	for(FRoleRequirement& Requirement : AllRoleRequirements)
	{
		if(InRole == Requirement.Role)
		{
			MaxCount = Requirement.MaxCount;
			break;
		}
	}
	
	if(GetNumInRoles(InRole) < MaxCount)
	{
		AEnemyBaseCharacter* Enemy = SelectWeightedRandomEnemy(InRole, AllEnemiesArray);

		if(Enemy)
		{
			UE_LOG(LogTemp, Warning, TEXT("Valid enemy"))
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No enemy"));
		}
		AssignEngagementRole(Enemy, InRole);
		// EnforceAllEngagementRoleCounts();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Already at maximum"))
	}
	
}
