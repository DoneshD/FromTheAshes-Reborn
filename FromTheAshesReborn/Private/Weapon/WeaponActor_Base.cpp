#include "Weapon/WeaponActor_Base.h"

AWeaponActor_Base::AWeaponActor_Base()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeaponActor_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponActor_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

