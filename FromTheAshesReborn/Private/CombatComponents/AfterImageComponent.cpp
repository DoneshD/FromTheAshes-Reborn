#include "CombatComponents/AfterImageComponent.h"

#include "FTACustomBase/AfterImageActor.h"
#include "FTACustomBase/FTACharacter.h"

UAfterImageComponent::UAfterImageComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

}


void UAfterImageComponent::BeginPlay()
{
	Super::BeginPlay();

	FTAChar = Cast<AFTACharacter>(GetOwner());
	
}

void UAfterImageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAfterImageComponent::CreateAfterImage()
{
	
	FVector SpawnLocation = FTAChar->GetMesh()->GetComponentLocation();
	FRotator SpawnRotation = FTAChar->GetMesh()->GetComponentRotation();

	AActor* Image = GetWorld()->SpawnActor<AActor>(FTAChar->AfterImageActor, SpawnLocation, SpawnRotation);

	if(!Image)
	{
		UE_LOG(LogTemp, Warning, TEXT("Image is Null"));
		return;
	}

	AAfterImageActor* AfterImage = Cast<AAfterImageActor>(Image);
	if(!AfterImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("AfterImageActor is Null"));
		return;
	}

	AfterImage->MeshToCopy = FTAChar->GetMesh();

	if(!AfterImage->MeshToCopy)
	{
		UE_LOG(LogTemp, Warning, TEXT("AfterImage->MeshToCopy is Null"));
		return;
	}

	AfterImage->SpawnAfterImageActor();
}

