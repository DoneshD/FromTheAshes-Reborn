#include "GameModes/FTAGameStateBase.h"

void AFTAGameStateBase::PublishEnemySpawned()
{
	//OnEnemySpawned.Execute();
}

void AFTAGameStateBase::PublishEnemyDeath()
{
	OnEnemyDeath.Execute();
}
