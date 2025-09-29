#include "Enemy/AIControllerEnemyGrunt.h"

#include "Navigation/CrowdFollowingComponent.h"

AAIControllerEnemyGrunt::AAIControllerEnemyGrunt(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAIControllerEnemyGrunt::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAIControllerEnemyGrunt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

