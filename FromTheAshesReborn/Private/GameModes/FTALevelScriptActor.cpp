#include "GameModes/FTALevelScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/EnemyBase.h"

void AFTALevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyBaseClass, EnemiesArray);
	
	EnemiesCount = EnemiesArray.Num();

	FTAGameStateBase = Cast<AFTAGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));

	FTAGameStateBase->OnEnemyDeath.BindUObject(this, &AFTALevelScriptActor::CheckEnemyCount);

}

void AFTALevelScriptActor::CheckEnemyCount()
{
	EnemiesCount -= 1;

	if (EnemiesCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("All Enemies Eliminated"));
	}
}
