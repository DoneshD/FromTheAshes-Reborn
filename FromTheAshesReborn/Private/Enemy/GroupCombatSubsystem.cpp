#include "Enemy/GroupCombatSubsystem.h"

#include "AbilitySystemComponent.h"
#include "StateTreeEvents.h"
#include "CombatComponents/GroupCombatComponent.h"
#include "Enemy/AIControllerEnemyBase.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "Enemy/FTAStateTreeAIComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/PlayerCharacter.h"

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
		const int32 RandomIndex =
			FMath::RandRange(0, CoverEnemies.Num() - 1);

		AEnemyBaseCharacter* NewAggressor =
			CoverEnemies[RandomIndex];

		UGroupCombatComponent* NewAggressorGCC =
			NewAggressor->FindComponentByClass<UGroupCombatComponent>();

		if (NewAggressorGCC)
		{
			NewAggressorGCC->EngagementRole =
				EEnemyEngagementRole::Aggressor;

			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Swapped aggressor: %s -> %s"),
				*InEnemy->GetName(),
				*NewAggressor->GetName()
			);

			if (AAIControllerEnemyBase* EnemyController =
				Cast<AAIControllerEnemyBase>(NewAggressor->GetController()))
			{
				const UFTAStateTreeAIComponent* STComp =
					EnemyController->StateTreeComponent;

				if (STComp)
				{
					FStateTreeEvent AttackEvent;
					AttackEvent.Tag =
						FGameplayTag::RequestGameplayTag(
							"StateTreeTag.State.Attacking");

					EnemyController->StateTreeComponent
						->SendStateTreeEvent(AttackEvent);
				}
			}
		}
	}
}

int32 UGroupCombatSubsystem::GetAggressorCount() const
{
	int32 Count = 0;

	for (AEnemyBaseCharacter* Enemy : AllEnemiesArray)
	{
		if (!Enemy)
		{
			continue;
		}

		const UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();

		if (GCC && GCC->EngagementRole == EEnemyEngagementRole::Aggressor)
		{
			Count += 1;
		}
	}

	return Count;
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

void UGroupCombatSubsystem::PrintAllAttackTokens()
{
	APlayerCharacter* PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	if (PC)
	{
		UGroupCombatComponent* PlayerGCC = PC->FindComponentByClass<UGroupCombatComponent>();
		if(PlayerGCC)
		{
			// PlayerGCC->AttackTokensCount = 1;
			// UE_LOG(LogTemp, Warning, TEXT("Player: %s - Count: %d"), *PC->GetName(), PlayerGCC->AttackTokensCount);
		}
	}
	
	for(auto Enemy : AllEnemiesArray)
	{
		if(Enemy)
		{
			UGroupCombatComponent* GCC = Enemy->FindComponentByClass<UGroupCombatComponent>();
			if(GCC)
			{
				// UE_LOG(LogTemp, Warning, TEXT("Enemy: %s - Count: %d"), *Enemy->GetName(), GCC->AttackTokensCount);
			}
		}
	}
}
