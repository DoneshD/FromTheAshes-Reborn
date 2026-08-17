#include "Level/EnemySpawner.h"

#include "NavigationSystem.h"
#include "DataAsset/EnemyEncounterDataAsset.h"
#include "Enemy/AIControllerEnemyBase.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "Enemy/EnemyCharacterDataAsset.h"
#include "Enemy/FTAStateTreeAIComponent.h"
#include "Enemy/GroupCombatSubsystem.h"
#include "GameModes/FTAGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StateTreeComponent.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemySpawner::SpawnEnemies(FWaveData InWaveData)
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		
	if(!NavSystem || !NavSystem->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("NavSystem is Null"));
		return;
	}

	for (const TPair<UEnemyCharacterDataAsset*, int32>& Element : InWaveData.EnemySpawnCount)
	{
		UEnemyCharacterDataAsset* EnemyData = Element.Key;
		int32 AmountToSpawn = Element.Value;

		for (int32 i = 0; i < AmountToSpawn; i++)
		{
			FNavLocation ResultLocation;
			bool bSuccess = NavSystem->GetRandomPointInNavigableRadius(GetActorLocation(), 1000, ResultLocation);

			if(bSuccess)
			{
				ResultLocation.Location = ResultLocation.Location + ResultLocation.Location.UpVector.GetSafeNormal() * VerticalLocationOffset;

				if (!EnemyData)
				{
					UE_LOG(LogTemp, Error, TEXT("EnemyData is null"));
					continue;
				}

				if (!EnemyData->EnemyCharacterClass)
				{
					UE_LOG(LogTemp, Error, TEXT("%s has a null EnemyCharacterClass"), *GetNameSafe(EnemyData));
					continue;
				}
				
				AEnemyBaseCharacter* Enemy = GetWorld()->SpawnActor<AEnemyBaseCharacter>(EnemyData->EnemyCharacterClass, ResultLocation.Location, FRotator(0, 0, 0));
				
				if(!Enemy)
				{
					UE_LOG(LogTemp, Warning, TEXT("Enemy null"))
					return;
				}
				
				
				if(Enemy)
				{
					if(!Enemy->GetController())
					{
						AAIControllerEnemyBase* EnemyController = GetWorld()->SpawnActor<AAIControllerEnemyBase>(EnemyData->EnemyControllerClass);
				
						if(EnemyController && EnemyController->IsValidLowLevel())
						{
							EnemyController->Possess(Enemy);
							AFTAGameModeBase* FTAGameMode = Cast<AFTAGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
							if(FTAGameMode)
							{
								if(FTAGameMode->ActivateAIBehavior)
								{
									EnemyController->StateTreeComponent->SetStateTree(EnemyData->StateTree);

									FStateTreeReference AttackStateTreeReference;
									AttackStateTreeReference.SetStateTree(EnemyData->LinkedPassive_StateTree);

									EnemyController->StateTreeComponent->AddLinkedStateTreeOverrides(EnemyData->TestTreeTag,
										AttackStateTreeReference
									);
							
								}
							}
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("Invalid AI controller"))
						}
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Already has controller"))
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Invalid enemy"))
				
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid location"))
			}
			
		}
	}
}

