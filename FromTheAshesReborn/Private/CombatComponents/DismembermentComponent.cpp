#include "CombatComponents/DismembermentComponent.h"

UDismembermentComponent::UDismembermentComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UDismembermentComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UDismembermentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UDismembermentComponent::CaptureDismembermentData(FName InBoneName)
{
	HitBoneName = InBoneName;
}

void UDismembermentComponent::RenameBoneName()
{
	switch (HitBoneName)
	{
	case FName("pelvis"):
		HitBoneName = "spine_01";
		break;

	case FName("spine_02"):
		HitBoneName = "spine_01";
		break;

	case FName("spine_03"):
		HitBoneName = "spine_01";
		break;

	case FName("neck_01"):
		HitBoneName = "spine_01";
		break;

	case FName("head"):
		HitBoneName = "spine_01";
		break;

	default:
		break;
	}
}

