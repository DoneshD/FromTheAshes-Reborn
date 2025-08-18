#include "CombatComponents/MeleePropertiesComponent.h"

#include "FTACustomBase/FTACharacter.h"
#include "TracingComponent/TracingComponent.h"
#include "Weapon/EquipmentManagerComponent.h"
#include "Weapon/WeaponActorBase.h"


UMeleePropertiesComponent::UMeleePropertiesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UMeleePropertiesComponent::BeginPlay()
{
	Super::BeginPlay();

	FTAChar = Cast<AFTACharacter>(GetOwner());

	if(!FTAChar)
	{
		UE_LOG(LogTemp, Error, TEXT("UMeleePropertiesComponent::BeginPlay - FTAChar is NULL"))
		return;
	}
	
}

void UMeleePropertiesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}