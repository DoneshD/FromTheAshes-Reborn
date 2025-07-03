#include "FTACustomBase/AfterImageActor.h"


AAfterImageActor::AAfterImageActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAfterImageActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAfterImageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAfterImageActor::InitiateSpawn()
{
	SpawnAfterImageActor();
}

