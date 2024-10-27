#include "ParkourSystem/WidgetActor.h"

AWidgetActor::AWidgetActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

