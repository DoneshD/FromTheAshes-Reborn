#include "Components/Movement/GroundedDashingCommponent.h"


UGroundedDashingCommponent::UGroundedDashingCommponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UGroundedDashingCommponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UGroundedDashingCommponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

