#include "Level/EnemySpawner.h"

#include "NavigationSystem.h"
#include "CombatComponents/GroupCombatComponent.h"
#include "Enemy/AIControllerEnemyGrunt.h"
#include "Enemy/EnemyBaseCharacter.h"
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

	OnActorBeginOverlap.AddDynamic(this, &AEnemySpawner::OnOverlapBegin);

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
	
	for(int32 i = 0; i < FTAGameMode->InitialEnemyCount; i++)
	{
		FNavLocation ResultLocation;
		bool bSuccess = NavSystem->GetRandomPointInNavigableRadius(GetActorLocation(), 1000, ResultLocation);

		if(bSuccess)
		{
			AEnemyGruntCharacter* Grunt = GetWorld()->SpawnActor<AEnemyGruntCharacter>(GruntClass, ResultLocation.Location, FRotator(0, 0, 0));

			if(Grunt)
			{
				if(!Grunt->GetController())
				{
					AAIControllerEnemyGrunt* GruntController = GetWorld()->SpawnActor<AAIControllerEnemyGrunt>(GruntControllerClass);

					if(GruntController && GruntController->IsValidLowLevel())
					{
						GruntController->Possess(Grunt);
						GruntController->StateTreeComponent->StartLogic();
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
	
	UE_LOG(LogTemp, Warning, TEXT("Done"));
	
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

void AEnemySpawner::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	
}

