#include "ParkourSystem/ArrowActor.h"

#include "Components/ArrowComponent.h"


AArrowActor::AArrowActor()
{
	PrimaryActorTick.bCanEverTick = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));

}

void AArrowActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArrowActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

