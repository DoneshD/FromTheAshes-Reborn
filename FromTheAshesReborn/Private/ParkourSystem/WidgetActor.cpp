#include "ParkourSystem/WidgetActor.h"
#include "Components/WidgetComponent.h"

AWidgetActor::AWidgetActor()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;  // Set the default scene root as the root component

	// Create and attach the WidgetComponent to the DefaultSceneRoot
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);  // Attach it to the DefaultSceneRoot
	
}

void AWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

