#include "Level/EnemySpawner.h"

#include "NavigationSystem.h"
#include "Enemy/AIControllerEnemyGrunt.h"
#include "Enemy/EnemyGruntCharacter.h"
#include "Enemy/GroupCombatSubsystem.h"
#include "GameModes/FTAGameModeBase.h"
#include "Kismet/GameplayStatics.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	
}

void AEnemySpawner::SpawnEnemies()
{
	AFTAGameModeBase* FTAGameMode = Cast<AFTAGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if(!FTAGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode is Null"));
		return;
	}

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		
	if(!NavSystem || !NavSystem->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("NavSystem is Null"));
		return;
	}
	
	for(int32 i = 0; i < 3; i++)
	{
		FNavLocation ResultLocation;
		bool bSuccess = NavSystem->GetRandomPointInNavigableRadius(GetActorLocation(), 1000, ResultLocation);

		if(bSuccess)
		{
			ResultLocation.Location = ResultLocation.Location + ResultLocation.Location.UpVector.GetSafeNormal() * VerticalLocationOffset;
			DrawDebugSphere(GetWorld(), ResultLocation.Location, 20, 10, FColor::Red, true);
			AEnemyGruntCharacter* Grunt = GetWorld()->SpawnActor<AEnemyGruntCharacter>(GruntClass, ResultLocation.Location, FRotator(0, 0, 0));

			if(Grunt)
			{
				if(!Grunt->GetController())
				{
					AAIControllerEnemyGrunt* GruntController = GetWorld()->SpawnActor<AAIControllerEnemyGrunt>(GruntControllerClass);

					if(GruntController && GruntController->IsValidLowLevel())
					{
						GruntController->Possess(Grunt);
						// GruntController->StateTreeComponent->StartLogic();
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Invalid grunt AI controller"))
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Invalid grunt controller"))
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid grunt actor"))
				
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid location"))
		}
	}
	
	UGroupCombatSubsystem* GCC = GetWorld()->GetSubsystem<UGroupCombatSubsystem>();

	if(!GCC)
	{
		UE_LOG(LogTemp, Error, TEXT("AEnemySpawner::BeginPlay() - Invalid group combat subsystem"))
		return;
	}
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

