
#include "PatrolRoute.h"
#include "Components/SplineComponent.h"

APatrolRoute::APatrolRoute()
{
	PrimaryActorTick.bCanEverTick = true;

	PatrolRoute = CreateDefaultSubobject<USplineComponent>(TEXT("PatrolRoute"));
	RootComponent = PatrolRoute;

}


void APatrolRoute::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APatrolRoute::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APatrolRoute::IncrementPatrolPointIndex()
{
	PatrolPointIndex = PatrolPointIndex + Direction;
	if (PatrolPointIndex == PatrolRoute->GetNumberOfSplinePoints() - 1)
	{
		Direction = -1;
	}
	else if (PatrolPointIndex == 0)
	{
		Direction = 1;
	}
}

FVector APatrolRoute::GetSplineAsWorldPosition()
{
	return PatrolRoute->GetLocationAtSplinePoint(PatrolPointIndex, ESplineCoordinateSpace::World);
}
