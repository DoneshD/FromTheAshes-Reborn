#include "Weapon/WeaponActorBase.h"
#include "DidItHitActorComponent.h"


AWeaponActorBase::AWeaponActorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	DidItHitActorComponent = CreateDefaultSubobject<UDidItHitActorComponent>("DidItHitActorComponent");
	this->AddOwnedComponent(DidItHitActorComponent);
	
}

void AWeaponActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

