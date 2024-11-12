
#include "Weapons/FTAEquipmentDefinition.h"
#include "Weapons/FTAEquipmentInstance.h"

UFTAEquipmentDefinition::UFTAEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
{
	InstanceType = UFTAEquipmentInstance::StaticClass();

}
