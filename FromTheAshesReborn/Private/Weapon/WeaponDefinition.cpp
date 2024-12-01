#include "Weapon/WeaponDefinition.h"
#include "Weapon/WeaponInstance.h"

UWeaponDefinition::UWeaponDefinition(const FObjectInitializer& ObjectInitializer)
{
	InstanceType = UWeaponInstance::StaticClass();

}
