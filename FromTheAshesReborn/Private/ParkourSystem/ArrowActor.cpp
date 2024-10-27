#include "ParkourSystem/ArrowActor.h"


AArrowActor::AArrowActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AArrowActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArrowActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

